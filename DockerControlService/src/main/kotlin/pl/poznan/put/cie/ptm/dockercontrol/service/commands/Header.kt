package pl.poznan.put.cie.ptm.dockercontrol.service.commands

data class Header (
    val sessionId: Int,
    val bodySize: Int
){
    companion object {
        private val headerRegEx = Regex("\\$[0-9]{3}+![0-9]{4}+!")
        fun isHeader(data: String) = headerRegEx.matches(data)
    }

}