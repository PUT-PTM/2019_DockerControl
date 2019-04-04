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
    val job: Job
    private val input = connection.openReadChannel()
    private val output = connection.openWriteChannel(autoFlush = true)


    init {
        job = GlobalScope.launch {
            try {
                delay(2500)
                val welcomePacket = Packet(id, Body(CMD.ACK)).make()
                Logger.log(welcomePacket)
                output.writeFully(welcomePacket.toByteArray())
                read()
            } catch (e: Throwable) {
                e.printStackTrace()
            } finally {
                connection.close()
            }
        }
    }

    private suspend fun read() {
        while (true) {
            val rawPacket = input.readPacket(Packet.HEADER_SIZE).readText()
            val response = try {
                processPacket(rawPacket)
            } catch (e: TCPServerException) {
                Logger.log("error: ${e.message}", this)
                Packet(id, Body(CMD.ERR, "${e.message}"))
            } finally {
                input.discard(input.availableForRead.toLong())
            }

            Logger.log(response.make(), this)
            output.writeFully(response.make().toByteArray())
        }
    }

    private suspend fun processPacket(packet: String): Packet {
        return processHeader(packet)
    }

    private suspend fun processHeader(rawHeader: String): Packet {
        Logger.log("header: $rawHeader", this)
        return if (Header.isHeader(rawHeader)) {
            val header = Packet.decodeHeader(rawHeader)
            if (header.sessionId == id) {
                val rawBody = input.readPacket(header.bodySize).readText()
                processBody(rawBody)
            } else throw TCPServerException("invalid session id")
        } else throw TCPServerException("invalid header")
    }

    private fun processBody(rawBody: String): Packet {
        Logger.log("body: $rawBody", this)
        return if (Body.isBody(rawBody)) {
            val body = Packet.decodeBody(rawBody)
            val response = Commands.process(body.cmd, body.data)
            Packet(id, Body(body.cmd, response))
        } else throw TCPServerException("invalid body")
    }

    internal class TCPServerException(msg: String) : Exception(msg)
}