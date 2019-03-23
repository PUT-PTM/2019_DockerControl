package pl.poznan.put.cie.ptm.dockercontrol.service.commands

object Command {
    // utils
    fun ack(data: String): String = "ACK"

    // containers
    fun getAllContainers(data: String) = "list of all containers"
    fun getActiveContainers(data: String) = "list of active"
    fun getContainerStats(data: String) = "stats of $data container"
}