package com.hiperbou.tiled

import com.fasterxml.jackson.core.JsonParser
import com.fasterxml.jackson.databind.DeserializationContext
import com.fasterxml.jackson.databind.DeserializationFeature
import com.fasterxml.jackson.databind.JsonDeserializer
import com.fasterxml.jackson.databind.MapperFeature
import com.fasterxml.jackson.dataformat.xml.JacksonXmlModule
import com.fasterxml.jackson.dataformat.xml.XmlMapper
import com.fasterxml.jackson.module.kotlin.readValue
import com.fasterxml.jackson.module.kotlin.registerKotlinModule
import java.io.File
import java.io.IOException

class NumericBooleanDeserializer : JsonDeserializer<Boolean?>() {
    @Throws(IOException::class)
    override fun deserialize(p: JsonParser, ctxt: DeserializationContext?): Boolean? {
        if ("1" == p.text) {
            return java.lang.Boolean.TRUE
        }
        return if ("0" == p.text) {
            java.lang.Boolean.FALSE
        } else null
    }
}

internal val kotlinXmlMapper = XmlMapper(JacksonXmlModule().apply {
    setDefaultUseWrapper(false)
    setXMLTextElementName("text")
}).registerKotlinModule()
    .configure(MapperFeature.ACCEPT_CASE_INSENSITIVE_PROPERTIES, true)
    .configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false)


internal inline fun <reified T : Any> parseAs(path: String): T {
    return kotlinXmlMapper.readValue(File(path))
}