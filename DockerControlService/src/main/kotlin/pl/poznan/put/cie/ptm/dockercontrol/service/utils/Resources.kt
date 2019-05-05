package pl.poznan.put.cie.ptm.dockercontrol.service.utils

import com.fasterxml.jackson.databind.JsonNode
import com.fasterxml.jackson.databind.ObjectMapper
import com.fasterxml.jackson.module.kotlin.KotlinModule

object Resources {
    private val config: JsonNode

    init {
        val objectMapper = ObjectMapper()
        objectMapper.registerModule(KotlinModule())

        val configFile = Resources::class.java.getResource("/config.json").readText()
        config = objectMapper.readTree(configFile)
    }

    private fun get(vararg field: String): JsonNode{
        var node = config.get(field.first())
        for (i in 1 until field.size) node = node.get(field[i])
        return node
    }

    fun getString(vararg field: String): String = get(*field).textValue()

    fun getInt(vararg field: String): Int = get(*field).intValue()
}
