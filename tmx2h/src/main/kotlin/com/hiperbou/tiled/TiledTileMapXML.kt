package com.hiperbou.tiled

import com.fasterxml.jackson.databind.annotation.JsonDeserialize
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlProperty
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlText

data class TiledTileMapXML (
    val compressionlevel: Long? = null,
    val height: Int? = null,
    @JsonDeserialize(using = NumericBooleanDeserializer::class)
    val infinite: Boolean? = null,
    @JacksonXmlProperty(localName = "layer")
    val layers: List<Layer> = emptyList(),
    val nextlayerid: Long? = null,
    val nextobjectid: Long? = null,
    val orientation: String? = null,
    val renderorder: String? = null,
    val tiledversion: String? = null,
    val tileheight: Int? = null,
    val tileset: List<Tileset>? = null,
    val tilewidth: Int? = null,
    val type: String? = null,
    val version: String? = null,
    val width: Int? = null
)

data class Tile(
    val gid:Int = 0
)

enum class Encoding { none, csv, base64 }
enum class Compression { none, gzip, zlib, zstd  }

data class TileData (
    val encoding:Encoding = Encoding.none,
    val compression:Compression = Compression.none,
    @JacksonXmlProperty(localName = "tile")
    val tiles: List<Tile> = emptyList(),
    @JacksonXmlProperty(localName = "text")
    @JacksonXmlText
    val value:String = ""
)

data class Layer (
    val data: TileData,
    val height: Int,
    val id: Int? = null,
    val name: String? = null,
    val opacity: Double? = null,
    val type: String? = null,
    val visible: Boolean? = null,
    val width: Int,
    val x: Int? = null,
    val y: Int? = null,
)

data class Tileset (
    val columns: Int? = null,
    val firstgid: Int? = null,
    val image: String? = null,
    val imageheight: Int? = null,
    val imagewidth: Int? = null,
    val margin: Int? = null,
    val name: String? = null,
    val spacing: Int? = null,
    val tilecount: Int? = null,
    val tileheight: Int? = null,
    val tilewidth: Int? = null
)