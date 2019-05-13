package pl.poznan.put.cie.ptm.dockercontrol.service.docker

import com.fasterxml.jackson.databind.DeserializationFeature
import com.fasterxml.jackson.databind.MapperFeature
import io.ktor.http.ContentType
import kotlinx.coroutines.runBlocking
import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.datatype.jdk8.Jdk8Module
import com.fasterxml.jackson.datatype.jsr310.JavaTimeModule
import com.fasterxml.jackson.module.kotlin.KotlinModule
import com.fasterxml.jackson.module.kotlin.readValue
import com.fasterxml.jackson.module.paramnames.ParameterNamesModule
import io.ktor.client.response.HttpResponse
import io.ktor.client.response.readText
import io.ktor.http.HttpStatusCode
import pl.poznan.put.cie.ptm.dockercontrol.service.docker.model.Container
import pl.poznan.put.cie.ptm.dockercontrol.service.docker.model.Image
import pl.poznan.put.cie.ptm.dockercontrol.service.docker.model.Stats
import pl.poznan.put.cie.ptm.dockercontrol.service.utils.Resources

class DockerConnector {

    private val mapper = ObjectMapper()

    private val http = Http(
        Resources.getString("docker", "host"),
        Resources.getInt("docker", "port")
    )

    init {
        mapper.registerModule(KotlinModule())
        mapper.registerModule(Jdk8Module())
        mapper.registerModule(JavaTimeModule())
        mapper.registerModule(ParameterNamesModule())

        mapper.configure(DeserializationFeature.ACCEPT_EMPTY_STRING_AS_NULL_OBJECT, true)
        mapper.configure(MapperFeature.ACCEPT_CASE_INSENSITIVE_PROPERTIES, true)
        mapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false)
    }

    fun ping() = runBlocking { http.get("_ping").status } == HttpStatusCode.OK

    fun getAllContainers() = getContainers(true)

    fun getRunningContainers() = getContainers(false)

    private fun getContainers(all: Boolean): List<Container> {
        val json = runBlocking {
            http.get("containers/json", mapOf("all" to "$all")).readText()
        }
        return mapper.readValue(json)
    }

    fun startContainer(id: String) = toggleContainer("start", id)

    fun stopContainer(id: String) = toggleContainer("stop", id)

    fun restartContainer(id: String) = toggleContainer("restart", id)

    private fun toggleContainer(operation: String, id: String): Boolean {
        val response = runBlocking { http.post("containers/$id/$operation") }
        return if (response.status == HttpStatusCode.NoContent) true
        else  TODO("handle $operation errors")
    }

    fun removeContainer(id: String): Boolean {
        val response = runBlocking { http.delete("containers/$id") }
        return if (response.status == HttpStatusCode.NoContent) true
        else  TODO("handle delete errors")
    }

    fun createContainer(image: String): Boolean {
        val response = containerCreateRequest(image)
        return if (response.status == HttpStatusCode.Created) true
        else TODO("handle errors [https://docs.docker.com/engine/api/v1.24/#create-a-container]")
    }

    private fun containerCreateRequest(image: String): HttpResponse {
        val body = mapper.writeValueAsString(mapOf("image" to image))
        return runBlocking {
            http.post(
                "containers/create",
                mapOf("name" to "${image.substringBefore(':')}-dc"),
                body,
                ContentType.Application.Json
            )
        }
    }

    fun getImages(): List<Image> {
        val json = runBlocking {
            http.get("images/json").readText()
        }
        return mapper.readValue(json)
    }

    fun getStats(): Stats {
        val json = runBlocking { http.get("info").readText() }
        return mapper.readValue(json)
    }
}
