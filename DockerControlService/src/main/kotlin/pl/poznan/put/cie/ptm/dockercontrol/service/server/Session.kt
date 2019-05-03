package pl.poznan.put.cie.ptm.dockercontrol.service.server

import io.ktor.network.sockets.Socket
import io.ktor.network.sockets.openReadChannel
import io.ktor.network.sockets.openWriteChannel
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.io.readPacket
import kotlinx.coroutines.io.writeFully
import kotlinx.coroutines.launch
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.Body
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.CMD
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.Commands
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.Header
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Logger

class Session (
    val id: Int,
    private val connection: Socket
) {
    var active = false
    val job: Job
    private val input = connection.openReadChannel()
    private val output = connection.openWriteChannel(autoFlush = true)

    init {
        job = GlobalScope.launch {
            try {
                read()
            } catch (e: Throwable) {
                e.printStackTrace()
            } finally {
                connection.close()
            }
        }
    }

    companion object {
        const val INACTIVE_SESSION_ID = 0
    }

    private suspend fun read() {
        while (true) {
            val rawPacket = input.readPacket(Packet.HEADER_SIZE).readText()

            val response = try {
                processPacket(rawPacket)
            } catch (e: SessionException) {
                Logger.log("error: ${e.message}", this)
                Packet(id, CMD.ERRR, "${e.message}")
            } finally {
                input.discard(input.availableForRead.toLong())
            }

            send(response)
        }
    }

    private suspend fun processPacket(packet: String): Packet = processHeader(packet)

    private suspend fun processHeader(rawHeader: String): Packet {
        Logger.log("header: $rawHeader", this)
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
        Logger.log("body: $rawBody", this)
        return if (Body.isBody(rawBody)) {
            val body = Packet.decodeBody(rawBody)
            val response = Commands.process(body.cmd, body.data, this)
            Packet(id, body.cmd, response)
        } else throw SessionException("invalid body")
    }

    private suspend fun send(packet: Packet) {
        val toSend = packet.make()
        Logger.log("sending: $toSend", this)
        delay(150)
        output.writeFully(toSend.toByteArray())
    }

    class SessionException(msg: String) : Exception(msg)
}