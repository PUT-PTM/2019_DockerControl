package pl.poznan.put.cie.ptm.dockercontrol.service.commands

data class Header (
    val sessionId: Int,
    val bodySize: Int
){
    companion object {
        const val SIZE = 10
        const val SESSION_ID_BEGIN = 1
        const val SESSION_ID_SIZE = 3
        const val BODY_SIZE_BEGIN = 5
        const val BODY_SIZE_SIZE = 4

        private val headerRegEx = Regex("\\$[0-9]{3}+![0-9]{4}+!")
        fun isHeader(data: String) = headerRegEx.matches(data)
    }

    constructor(rawHeader: String) : this (
        rawHeader.substring(SESSION_ID_BEGIN, SESSION_ID_BEGIN + SESSION_ID_SIZE).toInt(),
        rawHeader.substring(BODY_SIZE_BEGIN, BODY_SIZE_BEGIN + BODY_SIZE_SIZE).toInt()
    )

    fun toPacket(): String {
        val readyId = sessionId.toString().padStart(SESSION_ID_SIZE, '0')
        val readySize = bodySize.toString().padStart(BODY_SIZE_SIZE, '0')
        return "${Commands.PACKET_START}$readyId${Commands.PACKET_PART_DELIMITER}$readySize${Commands.PACKET_PART_DELIMITER}"
    }

}