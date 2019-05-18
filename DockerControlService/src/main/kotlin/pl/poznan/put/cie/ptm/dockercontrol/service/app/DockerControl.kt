package pl.poznan.put.cie.ptm.dockercontrol.service.app

import io.ktor.util.KtorExperimentalAPI
import kotlinx.coroutines.*
import pl.poznan.put.cie.ptm.dockercontrol.service.server.Session
import pl.poznan.put.cie.ptm.dockercontrol.service.server.TCPServer
import pl.poznan.put.cie.ptm.dockercontrol.service.service.AlertsService
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Logger
import java.time.Instant
import java.time.temporal.ChronoUnit

@KtorExperimentalAPI
object DockerControl {

    private val alertsService = AlertsService()

    private val sessions = mutableListOf<Session>()

    private const val SESSION_UPDATE_DELAY: Long = 60_000 // TODO: kill sessions inactive for triple CALL interval
    private const val ALERTS_UPDATE_DELAY: Long = 15_000

    fun run() = runBlocking {
        launch { acceptSessions() }
        launch { while (isActive) updateAlerts() }
        launch { while (isActive) updateSessions() }
        Logger.log("DockerControlService started")
    }

    private suspend fun acceptSessions() {
        val server = TCPServer()

        Logger.log("accepting tcp connections ${server.getAddress()}")
        coroutineScope { while (isActive)  {
            val session = server.accept(this)
            sessions.add(session)
        } }
    }

    private suspend fun updateAlerts() {
        Logger.log("updating alerts")
        val ids = alertsService.getAlerts()
        if (ids.isNotBlank()) sendAlerts(ids)
        delay(ALERTS_UPDATE_DELAY)
    }

    private suspend fun sendAlerts(ids: String) {
        sessions.forEach { it.sendAlert(ids) }
    }

    private suspend fun updateSessions() {
        Logger.log("updating sessions")
        sessions.forEach {
            if (it.longInactive()) {
                Logger.log("closing session ${it.id} due to inactivity")
                it.close()
            }
        }
        delay(SESSION_UPDATE_DELAY)
    }

    private fun Session.longInactive() = lastActivity.isBefore(Instant.now().minus(3, ChronoUnit.MINUTES))


}