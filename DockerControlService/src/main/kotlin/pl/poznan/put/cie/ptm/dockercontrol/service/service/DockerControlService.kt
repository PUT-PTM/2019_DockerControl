package pl.poznan.put.cie.ptm.dockercontrol.service.service

import pl.poznan.put.cie.ptm.dockercontrol.service.docker.DockerConnector
import pl.poznan.put.cie.ptm.dockercontrol.service.server.Session

class DockerControlService {

    private val connector = DockerConnector()

    // utils
    fun ready(data: String, session: Session): String {
        if (session.active) throw Session.SessionException("activating active session")
        session.active = true
        return ""
    }

    fun ack(data: String, session: Session): String = "ACKN"

    // containers
    fun getAllContainers(data: String, session: Session): String {
        return connector.getAllContainers().toString()
    }

    fun getActiveContainers(data: String, session: Session): String {
        return connector.getRunningContainers().toString()
    }

    fun getContainerStats(data: String, session: Session): String = "stats of $data container"

    fun startContainer(data: String, session: Session): String {
        val started = connector.startContainer(data)
        return "${if (started) 1 else 0},$data"
    }

    fun stopContainer(data: String, session: Session): String {
        val stopped = connector.stopContainer(data)
        return "${if (stopped) 1 else 0},$data"
    }

    fun restartContainer(data: String, session: Session): String {
        val restarted = connector.startContainer(data)
        return "${if (restarted) 1 else 0},$data"
    }

    fun removeContainer(data: String, session: Session): String {
        val removed = connector.removeContainer(data)
        return "${if (removed) 1 else 0},$data"
    }

    fun createContainer(data: String, session: Session): String {
        val created = connector.createContainer(data)
        return "${if (created) 1 else 0}"
    }

    fun getImages(data: String, session: Session): String {
        return connector.getImages().toString()
    }

    fun getStats(data: String, session: Session): String {
        return "${connector.getStats()}-${connector.getVersion()}"
    }
}