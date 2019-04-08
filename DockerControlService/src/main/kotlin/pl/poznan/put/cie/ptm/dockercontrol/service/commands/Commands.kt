package pl.poznan.put.cie.ptm.dockercontrol.service.commands

import pl.poznan.put.cie.ptm.dockercontrol.service.server.Session

object Commands {

    private val commands = hashMapOf(
        CMD.READ to Command::ready,
        CMD.ACKN to Command::ack,
        CMD.CALL to Command::getAllContainers,
        CMD.CATC to Command::getActiveContainers,
        CMD.CSTS to Command::getContainerStats
    )

    fun process(cmd: CMD, data: String, session: Session): String {
        return commands[cmd]?.invoke(data, session) ?: "cmd error"
    }

}