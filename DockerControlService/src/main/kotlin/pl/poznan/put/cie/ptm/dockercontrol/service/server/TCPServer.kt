package pl.poznan.put.cie.ptm.dockercontrol.service.server

import io.ktor.network.selector.ActorSelectorManager
import io.ktor.network.sockets.aSocket
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Logger
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Resources
import java.net.InetSocketAddress

object TCPServer {
    private val IP = Resources.getString("server", "ip")
    private val PORT = Resources.getInt( "server", "port")

    private val sessions =  mutableMapOf<Int, Session>()
    private var lastId = 0

    private val actorsSelectorManager = ActorSelectorManager(Dispatchers.IO)
    private val socketBuilder = aSocket(actorsSelectorManager).tcp()
    private val socket = socketBuilder.bind(InetSocketAddress(IP, PORT))

    fun start() {
        runBlocking {
            Logger.log("Started tcp server at ${socket.localAddress}")
            while (true) accept()
        }
    }

    private suspend fun accept() {
        val connection = socket.accept()

        val id = ++lastId
        Logger.log("new session: $id, remote: ${connection.remoteAddress}")
        val session = Session(id, connection)
        sessions[id] = session
    }
}
