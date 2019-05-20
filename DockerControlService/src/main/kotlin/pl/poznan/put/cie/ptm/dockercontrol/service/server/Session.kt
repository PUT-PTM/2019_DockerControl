package pl.poznan.put.cie.ptm.dockercontrol.service.server

import io.ktor.network.sockets.Socket
import io.ktor.network.sockets.openReadChannel
import io.ktor.network.sockets.openWriteChannel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.io.readPacket
import kotlinx.coroutines.io.writeFully
import kotlinx.coroutines.withContext
import pl.poznan.put.cie.ptm.dockercontrol.service.server.packet.Body
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.CMD
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.Commands
import pl.poznan.put.cie.ptm.dockercontrol.service.server.packet.Header
import pl.poznan.put.cie.ptm.dockercontrol.service.server.packet.Packet
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Logger
import java.time.Instant

class Session (
    val id: Int,
    private val connection: Socket
) {
    var active = false
    private set

    var on = true
    private set

    private val input = connection.openReadChannel()
    private val output = connection.openWriteChannel(autoFlush = true)

    var lastActivity: Instant = Instant.now()
    private set

    companion object {
        const val INACTIVE_SESSION_ID = 0
    }

    suspend fun start() {
        try {
            read()
        } catch (e: Throwable) {
            e.printStackTrace()
        } finally {
            withContext(Dispatchers.IO) { connection.close() }
            Logger.log("closed", this)
        }
    }

    private suspend fun read() {
        while (on) {
            val rawData = waitForData()
            val response = processData(rawData)
            send(response)
            lastActivity = Instant.now()
        }
    }

    private suspend fun waitForData() = input.readPacket(Packet.HEADER_SIZE).readText()

    private suspend fun processData(rawData: String): Packet {
        return try {
            processPacket(rawData)
        } catch (e: SessionException) {
            Logger.log("error: ${e.message}", this)
            Packet(id, CMD.ERRR, "${e.message}")
        } finally {
            input.discard(input.availableForRead.toLong())
        }
    }

    private suspend fun processPacket(packet: String): Packet = processHeader(packet)

    private suspend fun processHeader(rawHeader: String): Packet {
        Logger.log("received header: $rawHeader", this)
        return if (Header.isHeader(rawHeader)) {
            val header = Packet.decodeHeader(rawHeader)

            if (active && header.sessionId != id) throw SessionException("invalid session id")
            if (!active && header.sessionId != INACTIVE_SESSION_ID) throw SessionException("session inactive")

            processRawBody(header)
        } else throw SessionException("invalid header")
    }

    private suspend fun processRawBody(header: Header): Packet {
        val rawBody = input.readPacket(header.bodySize).readText()
        return processBody(rawBody)
    }

    private fun processBody(rawBody: String): Packet {
        Logger.log("received body: $rawBody", this)
        return if (Body.isBody(rawBody)) {
            val body = Packet.decodeBody(rawBody)
            val response = Commands.process(body.cmd, body.data, this)
            Packet(id, body.cmd, response)
        } else throw SessionException("invalid body")
    }

    private suspend fun send(packet: Packet) {
        val toSend = packet.make()
        Logger.log("sending: $toSend", this)
        delay(250)
        output.writeFully(toSend.toByteArray())
    }

    fun activate() { active = true }

    fun close() { active = false; on = false } // TODO: make session close immediately

    suspend fun sendAlert(ids: String) {
        val packet = Packet(id, CMD.ALRT, ids)
        send(packet)
    }

    class SessionException(msg: String) : Exception(msg)
}
