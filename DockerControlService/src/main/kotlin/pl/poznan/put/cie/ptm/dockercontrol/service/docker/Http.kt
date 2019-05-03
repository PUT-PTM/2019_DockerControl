package pl.poznan.put.cie.ptm.dockercontrol.service.docker

import io.ktor.client.HttpClient
import io.ktor.client.request.parameter
import io.ktor.client.request.request
import io.ktor.client.request.url
import io.ktor.client.response.HttpResponse
import io.ktor.http.ContentType
import io.ktor.http.HttpMethod
import io.ktor.http.content.TextContent

class Http (
        private val host: String,
        private val port: Int
) {
    private val client = HttpClient()

    suspend fun get(
        path: String,
        parameters: Map<String, String> = emptyMap()
    ) = request(path, HttpMethod.Get, parameters)

    suspend fun post(
        path: String,
        parameters: Map<String, String> = emptyMap(),
        body: String = "",
        contentType: ContentType = ContentType.Application.Json
    ) = request(path, HttpMethod.Post, parameters, body, contentType)

    suspend fun delete(
        path: String
    ) = request(path, HttpMethod.Delete)

    private suspend fun request(
        path: String,
        method: HttpMethod,
        parameters: Map<String, String> = emptyMap(),
        body: String = "",
        contentType: ContentType = ContentType.Any
    ): HttpResponse {
        return client.request {
            url("http://$host:$port/$path")
            this.method = method
            this.body = TextContent(body, contentType)
            parameters.forEach { this.parameter(it.key, it.value) }
        }
    }

    fun close() = client.close()

}