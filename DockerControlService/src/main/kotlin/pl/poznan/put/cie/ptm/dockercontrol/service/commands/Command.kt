package pl.poznan.put.cie.ptm.dockercontrol.service.commands

import pl.poznan.put.cie.ptm.dockercontrol.service.server.Session

object Command {
    // utils
    fun ready(data: String, session: Session): String {
        if (session.active) throw Session.SessionException("activating active session")
        session.active = true
        return ""
    }

    fun ack(data: String, session: Session): String = "ACKN"

    // containers
    fun getAllContainers(data: String, session: Session) = "list of all containers"
    fun getActiveContainers(data: String, session: Session) = "list of active"
    fun getContainerStats(data: String, session: Session) = "stats of $data container"
}