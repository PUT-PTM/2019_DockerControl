package pl.poznan.put.cie.ptm.dockercontrol.service.server

import pl.poznan.put.cie.ptm.dockercontrol.service.commands.Body
import pl.poznan.put.cie.ptm.dockercontrol.service.commands.Header

data class Packet(
    val sessionId: Int,
    val body: Body
) {
    fun make(): String {
        val bodyPacket = body.toPacket()
        val headerPacket = Header(sessionId, bodyPacket.length).toPacket()
        return "$headerPacket$bodyPacket"
    }
}