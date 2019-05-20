package pl.poznan.put.cie.ptm.dockercontrol.service.service

import pl.poznan.put.cie.ptm.dockercontrol.service.docker.DockerConnector
import pl.poznan.put.cie.ptm.dockercontrol.service.docker.model.Container

class AlertsService {

    private val connector = DockerConnector()
    private val containers = mutableMapOf<String, Boolean>()

    companion object {
        private const val STATE_RUNNING = "running"
    }

    fun getAlerts(): String {
        val current = connector.getAllContainers()
        val down = updateContainers(current)
        val removed = checkRemoved(current)

        val ids = down
            .union(removed)
            .toList()

        return if (ids.isNotEmpty()) DataSerializer.toIdsArray(ids)
        else ""
    }

    private fun updateContainers(current: List<Container>): List<String> {
        val alerts = mutableListOf<String>()
        current.forEach { if (updateContainer(it)) alerts.add(it.id) }
        return alerts
    }

    private fun updateContainer(container: Container): Boolean {
        val isRunning = container.state.toLowerCase() == STATE_RUNNING
        var stateChanged = false

        if (containers.containsKey(container.id)) {
            if (containers[container.id]!! && !isRunning) {
                stateChanged = true
            }
        }

        containers[container.id] = isRunning
        return stateChanged
    }

    private fun checkRemoved(current: List<Container>): List<String> {
        val currentIds = current
            .map { it.id }
            .toSet()

        val removed = mutableListOf<String>()
        containers.keys.forEach { if( !currentIds.contains(it)) removed.add(it) }
        removed.forEach { containers.remove(it) }

        return removed
    }
}