package com.hiperbou.tiled

import java.io.ByteArrayOutputStream
import java.nio.ByteBuffer
import java.nio.ByteOrder
import java.util.zip.Inflater
import kotlin.io.encoding.Base64.Default.decode
import kotlin.io.encoding.ExperimentalEncodingApi

class TileMapLayer (
    val name:String,
    val numCols:Int,
    val numRows:Int,
    val map:IntArray
) {
    companion object{
        fun fromTiledTileMapLayer(it:Layer, mapper: TileMapper):TileMapLayer {
            return when(it.data.encoding) {
                Encoding.csv -> fromCSVLayer(it, mapper)
                Encoding.base64 -> {
                    when (it.data.compression) {
                        Compression.none -> fromBase64Layer(it, mapper)
                        Compression.zlib -> fromBase64ZLibLayer(it, mapper)
                        else -> throw Exception("Compressed Base64(${it.data.compression} compressed) format not supported")
                    }
                }
                else -> fromXMLLayer(it, mapper)
            }
        }

        private fun fromXMLLayer(it:Layer, mapper: TileMapper):TileMapLayer {
            val map = it.data.tiles.map { mapper.map(it.gid) }.toIntArray()
            return TileMapLayer(it.name!!, it.width, it.height, map)
        }

        private fun fromCSVLayer(it:Layer, mapper: TileMapper):TileMapLayer {
            val map = it.data.value.replace("\n", "").split(",").map { mapper.map(it.toInt()) }.toIntArray()
            return TileMapLayer(it.name!!, it.width, it.height, map)
        }

        @OptIn(ExperimentalEncodingApi::class)
        private fun fromBase64Layer(it:Layer, mapper: TileMapper):TileMapLayer {
            val map = decode(it.data.value.replace("\n","").trim().toByteArray()).toIntArray().map { mapper.map(it) }.toIntArray()
            return TileMapLayer(it.name!!, it.width, it.height, map)
        }

        @OptIn(ExperimentalEncodingApi::class)
        private fun fromBase64ZLibLayer(it:Layer, mapper:TileMapper):TileMapLayer {
            fun decompress(input: ByteArray) : ByteArray {
                val inflater = Inflater()
                val outputStream = ByteArrayOutputStream()
                val buffer = ByteArray(1024)

                inflater.setInput(input)

                while (!inflater.finished()) {
                    val count = inflater.inflate(buffer)
                    outputStream.write(buffer, 0, count)
                }

                outputStream.close()
                return outputStream.toByteArray()
            }
            val map = decompress(decode(it.data.value.replace("\n","").trim().toByteArray())).toIntArray().map { mapper.map(it) }.toIntArray()
            return TileMapLayer(it.name!!, it.width, it.height, map)
        }

        private fun toInt32(bytes: ByteArray, index: Int): Int {
            return ByteBuffer.wrap(bytes, index, 4).order(ByteOrder.LITTLE_ENDIAN).int
        }

        private fun ByteArray.toIntArray():IntArray {
            val result = IntArray(size/4)
            for ((n, i) in (indices step 4).withIndex()) {
                result[n] = toInt32(this, i)
            }
            return result
        }
    }
}

