package pl.poznan.put.cie.ptm.dockercontrol.service.commands

data class Body (
    val cmd: CMD,
    val data: String
) {
    companion object {
        private val bodyRegEx = Regex("[A-Z]+![^#]*#")
        fun isBody(data: String) = bodyRegEx.matches(data)
    }

    constructor(cmd: String, data: String) : this(CMD.valueOf(cmd), data)

    constructor(rawBody: String) : this(
        rawBody.substringBefore("!"),
        rawBody.substringAfter("!").substringBefore("#")
    )

    fun toPacket(): String = "$cmd${Commands.PACKET_PART_DELIMITER}$data${Commands.PACKET_END}"
}