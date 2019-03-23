package pl.poznan.put.cie.ptm.dockercontrol.service.commands

object Commands {

    const val PACKET_START = '$'
    const val PACKET_END = '#'
    const val PACKET_PART_DELIMITER = '!'

    private val commands = hashMapOf(
        CMD.ACK to Command::ack,
        CMD.CALL to Command::getAllContainers,
        CMD.CATC to Command::getActiveContainers,
        CMD.CSTS to Command::getContainerStats
    )

    fun process(cmd: CMD, data: String): String {
        return commands[cmd]?.invoke(data) ?: "cmd error"
    }

}