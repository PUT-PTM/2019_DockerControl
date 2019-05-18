package pl.poznan.put.cie.ptm.dockercontrol.service.server

import io.ktor.network.selector.ActorSelectorManager
import io.ktor.network.sockets.aSocket
import kotlinx.coroutines.*
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Logger
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Resources
import java.net.InetSocketAddress
import io.ktor.network.sockets.Socket
import io.ktor.util.KtorExperimentalAPI
import pl.poznan.put.cie.ptm.dockercontrol.service.service.AlertsService
import java.time.Instant
import java.time.temporal.ChronoUnit

@KtorExperimentalAPI
class TCPServer {
    private val ip = Resources.getString("server", "ip")
    private val port = Resources.getInt( "server", "port")

    private val sessions =  mutableMapOf<Int, Session>()
    private var lastId = 0

    private val actorsSelectorManager = ActorSelectorManager(Dispatchers.IO)
    private val socketBuilder = aSocket(actorsSelectorManager).tcp()
    private val socket = socketBuilder.bind(InetSocketAddress(ip, port))

    suspend fun accept(scope: CoroutineScope): Session {
        val connection = socket.accept()
        return createSession(connection, scope)
    }

    private fun createSession(connection: Socket, scope: CoroutineScope): Session {
        val id = ++lastId
        val session = Session(id, connection)
        sessions[id] = session

        scope.launch { session.start() }
        Logger.log("new session: $id, remote: ${connection.remoteAddress}")
        return session
    }

    fun getAddress() = socket.localAddress
}
