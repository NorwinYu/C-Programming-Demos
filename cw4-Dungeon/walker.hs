-- Dungeon walker
-- Loads in a dungeon map from a file.
-- Then reads coordinates from stdin to show the
-- position of the player in the dungeon each turn.  This program does
-- nothing to validate the positions, it just accepts them and displays the
-- player position there.
-- Beware, there is not a great deal of error checking here...
-- 20161122 Paul Dempster created.

import System.Environment (getArgs)

--------------------------------------------------------------------------------
-- Types for individual cells and the overall dungeon.
-- Include 2 simple show instances so representations can be automatically
-- printed, although we only use them for debugging.

-- Different types of cells.
data Cell = Space | Wall | Exit        

instance Show Cell where
  show Space = "."
  show Wall = "#"
  show Exit = "x"

-- Width, height, 2D cell data.
data Dungeon = Dungeon Int Int [[Cell]]

instance Show Dungeon where
  show (Dungeon w h rows) =  unlines (map f1 (rows))
    where
      f1 = (\row -> concat (map f2 (row)))
      f2 = (\cell -> show cell)

--------------------------------------------------------------------------------
-- Display functions for dungeon, which includes the player character overlaid
-- in the given position.

-- Doing all this zipping each time is inefficient, should have just stored coord in cells.
displayDungeon :: Dungeon -> Int -> Int -> IO ()
displayDungeon (Dungeon _ _ rows) px py = mapM_ (displayDungeonRow px py) (zip [0..] rows)


displayDungeonRow :: Int -> Int -> (Int, [Cell]) -> IO ()
displayDungeonRow px py (ry, row) =
  do mapM_ (displayDungeonCell px py ry) (zip [0..] row)
     putStrLn ""

displayDungeonCell :: Int -> Int -> Int -> (Int, Cell) -> IO ()
displayDungeonCell px py ry (rx, cell) = if px == rx && py == ry
                                         then putStr "@"
                                         else putStr (show cell)
                                    

--------------------------------------------------------------------------------
-- Parsing the dungeon file
parseDungeon :: String -> Dungeon
parseDungeon str = Dungeon width height map
  where
    strLines = lines str
    headerLine = head strLines
    mapLines = tail strLines
    (width, height) = parseHeader headerLine
    map = parseMap width height mapLines

parseHeader :: String -> (Int, Int)
parseHeader header = (width, height)
  where
    ws = words header
    width = read (ws!!0)
    height = read (ws!!1)

parseMap :: Int -> Int -> [String] -> [[Cell]]
parseMap w h rows = map (parseRow w) rows

parseRow :: Int                 -- Width of the row
         -> String              -- Text representation of the row
         -> [Cell]              -- Parsed representation
parseRow w str = map parseCell str

parseCell :: Char -> Cell
parseCell '.' = Space
parseCell '#' = Wall
parseCell 'x' = Exit

-- Parsing the coords from standard input
parseCoord :: String -> (Int, Int)
parseCoord str = (read xstr, read (tail ystr))
  where
    (xstr, ystr) = (span (/= ',') str)


--------------------------------------------------------------------------------
-- main loop
main :: IO ()
main = do fn <- processArgs
          dungeon <- loadDungeon fn
          putStrLn "Waiting for first input..."
          doStep (parseDungeon dungeon)

doStep :: Dungeon -> IO ()
doStep dungeon =
  do input <- getLine
     if length input == 0
       then putStrLn "End of input."
       else do let (nx, ny) = parseCoord input
               putStrLn ((show nx) ++ "," ++ (show ny))
               displayDungeon dungeon nx ny
               doStep dungeon
     
-- Get the filename from the command line args or exit with an error message.
processArgs :: IO (String)
processArgs = do args <- getArgs
                 if length args /= 1
                   then error "Wrong number of filenames"
                   else return (args!!0)


loadDungeon :: String -> IO (String)
loadDungeon fn = do contents <- readFile fn
                    return contents
