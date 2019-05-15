package pl.poznan.put.cie.ptm.dockercontrol.service.server

import io.ktor.network.selector.ActorSelectorManager
import io.ktor.network.sockets.aSocket
import kotlinx.coroutines.*
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Logger
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Resources
import java.net.InetSocketAddress
import io.ktor.network.sockets.Socket

object TCPServer {
    private val IP = Resources.getString("server", "ip")
    private val PORT = Resources.getInt( "server", "port")

    private val sessions =  mutableMapOf<Int, Session>()
    private var lastId = 0

    private val actorsSelectorManager = ActorSelectorManager(Dispatchers.IO)
    private val socketBuilder = aSocket(actorsSelectorManager).tcp()
    private val socket = socketBuilder.bind(InetSocketAddress(IP, PORT))

    fun start() = runBlocking {
        Logger.log("Started tcp server at ${socket.localAddress}")
        coroutineScope { while (true) accept(this) }
    }

    private suspend fun accept(scope: CoroutineScope) {
        val connection = socket.accept()
        createSession(connection, scope)
    }

    private fun createSession(connection: Socket, scope: CoroutineScope) {
        val id = ++lastId
        val session = Session(id, connection)
        sessions[id] = session

        scope.launch { session.start() }
        Logger.log("new session: $id, remote: ${connection.remoteAddress}")
    }
}
