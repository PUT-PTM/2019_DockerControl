package pl.poznan.put.cie.ptm.dockercontrol.service.server

import pl.poznan.put.cie.ptm.dockercontrol.service.server.packet.Body
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.CMD
import pl.poznan.put.cie.ptm.dockercontrol.service.server.packet.Header

data class Packet(
    val sessionId: Int,
    val body: Body
) {

    companion object {
        const val PACKET_START = '$'
        const val PACKET_END = '#'
        const val PACKET_PART_DELIMITER = '!'

        const val HEADER_SIZE = 10
        const val SESSION_ID_BEGIN = 1
        const val SESSION_ID_SIZE = 3
        const val BODY_SIZE_BEGIN = 5
        const val BODY_SIZE_SIZE = 4

        fun decodeBody(rawBody: String) = Body(
            rawBody.substringBefore(PACKET_PART_DELIMITER),
            rawBody.substringAfter(PACKET_PART_DELIMITER).substringBefore(PACKET_END)
        )

        fun decodeHeader(rawHeader: String) = Header(
            rawHeader.substring(SESSION_ID_BEGIN, SESSION_ID_BEGIN + SESSION_ID_SIZE).toInt(),
            rawHeader.substring(BODY_SIZE_BEGIN, BODY_SIZE_BEGIN + BODY_SIZE_SIZE).toInt()
        )
    }

    constructor(sessionId: Int, cmd: CMD, body: String = "") : this(sessionId,
        Body(cmd, body)
    )

    fun make(): String {
        val bodyPacket = "${body.cmd}$PACKET_PART_DELIMITER${body.data}$PACKET_END"

        val readyId = sessionId.toString().padStart(SESSION_ID_SIZE, '0')
        val readySize = bodyPacket.length.toString().padStart(BODY_SIZE_SIZE, '0')
        val headerPacket = "$PACKET_START$readyId$PACKET_PART_DELIMITER$readySize$PACKET_PART_DELIMITER"

        return "$headerPacket$bodyPacket"
    }
}