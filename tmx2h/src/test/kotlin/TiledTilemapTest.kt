import com.hiperbou.tiled.*
import com.hiperbou.tiled.parseAs
import kotlin.test.*

class TiledTileMapXMLTest {
    @Test
    fun tiledTileMapXMLTest() {
        val tiledTileMapXML = parseAs<TiledTileMapXML>("map-xml.tmx")
        with(tiledTileMapXML.layers[0].data) {
            assertTrue { encoding == Encoding.none }
            assertTrue { value.isEmpty() }
            assertTrue { tiles.isNotEmpty() }
        }
    }

    @Test
    fun tiledTileMapCSVTest() {
        val tiledTileMapXML = parseAs<TiledTileMapXML>("map-csv.tmx")
        with(tiledTileMapXML.layers[0].data) {
            assertTrue { encoding == Encoding.csv }
            assertTrue { value.isNotEmpty() }
            assertTrue { tiles.isEmpty() }
        }
    }

    @Test
    fun tiledTileMapBase64Test() {
        val tiledTileMapBase64 = parseAs<TiledTileMapXML>("map-base64.tmx")
        with(tiledTileMapBase64.layers[0].data) {
            assertTrue { encoding == Encoding.base64 }
            assertTrue { compression == Compression.none }
            assertTrue { value.isNotEmpty() }
            assertTrue { tiles.isEmpty() }
        }

        val tileMapLayer = TileMapLayer.fromTiledTileMapLayer(tiledTileMapBase64.layers[0], TileMapper(mapOf()))
        assertTrue { tileMapLayer.map.isNotEmpty() }

        val tiledTileMapXML = parseAs<TiledTileMapXML>("map-csv.tmx")
        val tileMapLayer2 = TileMapLayer.fromTiledTileMapLayer(tiledTileMapXML.layers[0], TileMapper(mapOf()))

        assertContentEquals(tileMapLayer2.map, tileMapLayer.map)
    }

    @Test
    fun tiledTileMapBase64ZLibTest() {
        val tiledTileMapBase64ZLib = parseAs<TiledTileMapXML>("map-base64zlib.tmx")
        with(tiledTileMapBase64ZLib.layers[0].data) {
            assertTrue { encoding == Encoding.base64 }
            assertTrue { compression == Compression.zlib }
            assertTrue { value.isNotEmpty() }
            assertTrue { tiles.isEmpty() }
        }

        val tileMapLayer = TileMapLayer.fromTiledTileMapLayer(tiledTileMapBase64ZLib.layers[0], TileMapper(mapOf()))
        assertTrue { tileMapLayer.map.isNotEmpty() }

        val tiledTileMapXML = parseAs<TiledTileMapXML>("map-csv.tmx")
        val tileMapLayer2 = TileMapLayer.fromTiledTileMapLayer(tiledTileMapXML.layers[0], TileMapper(mapOf()))

        assertContentEquals(tileMapLayer2.map, tileMapLayer.map)
    }
}