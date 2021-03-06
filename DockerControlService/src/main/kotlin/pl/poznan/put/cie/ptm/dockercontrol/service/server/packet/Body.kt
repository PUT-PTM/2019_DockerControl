package pl.poznan.put.cie.ptm.dockercontrol.service.server.packet

import pl.poznan.put.cie.ptm.dockercontrol.service.commands.CMD

data class Body (
    val cmd: CMD,
    val data: String = ""
) {
    companion object {
        private val bodyRegEx = Regex("[A-Z]+![^#]*#")
        fun isBody(data: String) = bodyRegEx.matches(data)
    }

    constructor(cmd: String, data: String) : this(CMD.valueOf(cmd), data)
}
