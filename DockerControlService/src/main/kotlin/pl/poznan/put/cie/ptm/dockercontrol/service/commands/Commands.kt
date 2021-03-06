package pl.poznan.put.cie.ptm.dockercontrol.service.commands

import pl.poznan.put.cie.ptm.dockercontrol.service.server.Session
import pl.poznan.put.cie.ptm.dockercontrol.service.service.DockerService

object Commands {

    private val service = DockerService()

    private val commands = hashMapOf(
        // utils
        CMD.READ to service::ready,
        CMD.ACKN to service::ack,

        // containers
        CMD.CALL to service::getAllContainers,
        CMD.CACT to service::getActiveContainers,
        CMD.CSTS to service::getContainerStats,
        CMD.CSTR to service::startContainer,
        CMD.CSTP to service::stopContainer,
        CMD.CRST to service::restartContainer,
        CMD.CRMV to service::removeContainer,
        CMD.CCRT to service::createContainer,

        // images
        CMD.IALL to service::getImages,

        // system
        CMD.SSTS to service::getStats
    )

    fun process(cmd: CMD, data: String, session: Session) = commands[cmd]?.invoke(data, session) ?: "cmd error"

}
