package pl.poznan.put.cie.ptm.dockercontrol.service.utils

import pl.poznan.put.cie.ptm.dockercontrol.service.server.Session
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

object Logger {
    private val formatter = DateTimeFormatter.ofPattern("HH:mm:ss:SS")

    fun log(msg: String) {
        val time = formatter.format(LocalDateTime.now())
        println("$time - $msg")
    }

    fun log(msg: String, session: Session) = log("s: ${session.id} - $msg")
}
