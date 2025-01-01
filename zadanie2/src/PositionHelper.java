import java.util.Random;
import java.util.Vector;

public class PositionHelper{
    public static MoveDirection[] MoveDirectionEnumValues = MoveDirection.class.getEnumConstants();
    public static MoveDirection GetRandomMoveDirection() {
        return MoveDirectionEnumValues[new Random().nextInt(MoveDirectionEnumValues.length)];
    }

    public static Position2D GetRightUpper(Position2D position2D){
        return new Position2D(position2D.col() + 1, position2D.row() + 1);
    }
    public  static Position2D GetLeftLower(Position2D position2D){
        return new Position2D(position2D.col() - 1, position2D.row() - 1);
    }
    public  static Position2D GetRightLower(Position2D position2D){
        return new Position2D(position2D.col() + 1, position2D.row() - 1);
    }
    public  static Position2D GetLeftUpper(Position2D position2D){
        return new Position2D(position2D.col() - 1, position2D.row() + 1);
    }
    public  static Position2D GetLowMid(Position2D position2D){
        return new Position2D(position2D.col(), position2D.row() - 1);
    }
    public  static Position2D GetHighMid(Position2D position2D){
        return new Position2D(position2D.col(), position2D.row() + 1);
    }
    public  static Position2D GetRightMid(Position2D position2D){
        return new Position2D(position2D.col() + 1, position2D.row());
    }
    public  static Position2D GetLeftMid(Position2D position2D){
        return new Position2D(position2D.col() - 1, position2D.row());
    }

    public static Position2D[] GetAllNeighbours(Position2D position2D){
        return new Position2D[]{GetLowMid(position2D), GetRightLower(position2D), GetRightMid(position2D),
                GetRightUpper(position2D), GetHighMid(position2D), GetLeftUpper(position2D),
                GetLeftMid(position2D), GetLeftLower(position2D)};
    }

    public static  CellNeighbourhood GetCurrNeighbourhood(Position2D newPosition, Table2D table){
        int col = newPosition.col();
        int row = newPosition.row();
        int currLeftUpper=Integer.MIN_VALUE, currRightUpper=Integer.MIN_VALUE, currLeftLower=Integer.MIN_VALUE,
                currRightLower=Integer.MIN_VALUE,currLowMid=Integer.MIN_VALUE,currHighMid=Integer.MIN_VALUE,
                currRightMid=Integer.MIN_VALUE,currLeftMid = Integer.MIN_VALUE;
        if(row == 0){
            // 1
            if(col == 0){
                currHighMid = table.get(PositionHelper.GetHighMid(newPosition));
                currRightUpper = table.get(PositionHelper.GetRightUpper(newPosition));
                currRightMid = table.get(PositionHelper.GetRightMid(newPosition));
            }
            //2
            else if(col == table.cols() - 1){
                currHighMid = table.get(PositionHelper.GetHighMid(newPosition));
                currLeftUpper = table.get(PositionHelper.GetLeftUpper(newPosition));
                currLeftMid = table.get(PositionHelper.GetLeftMid(newPosition));
            }
            // 3
            else{
                currLeftMid = table.get(PositionHelper.GetLeftMid(newPosition));
                currLeftUpper = table.get(PositionHelper.GetLeftUpper(newPosition));
                currHighMid = table.get(PositionHelper.GetHighMid(newPosition));
                currRightUpper = table.get(PositionHelper.GetRightUpper(newPosition));
                currRightMid = table.get(PositionHelper.GetRightMid(newPosition));
            }
        }
        else if(row == table.rows()){
            // 4
            if(col == 0){
                currLowMid = table.get(PositionHelper.GetLowMid(newPosition));
                currRightMid = table.get(PositionHelper.GetRightMid(newPosition));
                currRightLower = table.get(PositionHelper.GetRightLower(newPosition));
            }
            // 5
            else if(col == table.cols() - 1){
                currLeftMid = table.get(PositionHelper.GetLeftMid(newPosition));
                currLeftLower = table.get(PositionHelper.GetLeftLower(newPosition));
                currLowMid = table.get(PositionHelper.GetLowMid(newPosition));
            }
            //  6
            else{
                currLeftMid = table.get(PositionHelper.GetLeftMid(newPosition));
                currLeftLower = table.get(PositionHelper.GetLeftLower(newPosition));
                currLowMid = table.get(PositionHelper.GetLowMid(newPosition));
                currRightLower = table.get(PositionHelper.GetRightLower(newPosition));
                currRightMid = table.get(PositionHelper.GetRightMid(newPosition));
            }
        }
        // 0 < row < max_row
        else{
            // 7
            if(col == 0){
                currHighMid = table.get(PositionHelper.GetHighMid(newPosition));
                currRightUpper = table.get(PositionHelper.GetRightUpper(newPosition));
                currRightMid = table.get(PositionHelper.GetRightMid(newPosition));
                currRightLower = table.get(PositionHelper.GetRightLower(newPosition));
                currLowMid = table.get(PositionHelper.GetLowMid(newPosition));
            }
            // 8
            else if(col == table.cols() - 1){
                currHighMid = table.get(PositionHelper.GetHighMid(newPosition));
                currLeftUpper = table.get(PositionHelper.GetLeftUpper(newPosition));
                currLeftMid = table.get(PositionHelper.GetLeftMid(newPosition));
                currLeftLower = table.get(PositionHelper.GetLeftLower(newPosition));
                currLowMid = table.get(PositionHelper.GetLowMid(newPosition));
            }
            // 0 < row < max_row && 0 < col < max_col
            else{
                currLeftUpper = table.get(PositionHelper.GetLeftUpper(newPosition));
                currHighMid = table.get(PositionHelper.GetHighMid(newPosition));
                currRightUpper = table.get(PositionHelper.GetRightUpper(newPosition));
                currRightMid = table.get(PositionHelper.GetRightMid(newPosition));
                currRightLower = table.get(PositionHelper.GetRightLower(newPosition));
                currLowMid = table.get(PositionHelper.GetLowMid(newPosition));
                currLeftLower = table.get(PositionHelper.GetLeftLower(newPosition));
                currLeftMid = table.get(PositionHelper.GetLeftMid(newPosition));
            }
        }
        return new CellNeighbourhood(currLeftUpper, currRightUpper,currLeftLower,currRightLower,currLowMid,currHighMid,currRightMid,currLeftMid);
    }

    // zakladamy brak kolizji
    public static Vector<Pair> FindPairs(Table2D table, Position2D currPosition,
                                 CellNeighbourhood currNeighbourhood, int valueToFind, MoveDirection moveDirection){
        boolean skipNextOnRoadFlag = false;
        Vector<Pair> result = new Vector<>();
        int currPosValue = table.get(currPosition);
        // ze srodka
        if(currPosValue + currNeighbourhood.currLeftUpper() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetLeftUpper(currPosition)));
        }
        else if(currPosValue + currNeighbourhood.currRightUpper() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetRightUpper(currPosition)));
        }
        else if(currPosValue + currNeighbourhood.currLeftLower() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetLeftLower(currPosition)));
        }
        else if(currPosValue + currNeighbourhood.currRightLower() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetRightLower(currPosition)));
        }
        else if(currPosValue + currNeighbourhood.currLowMid() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetLowMid(currPosition)));
            if(moveDirection == MoveDirection.Down)
                skipNextOnRoadFlag=true;
        }
        else if(currPosValue + currNeighbourhood.currHighMid() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetHighMid(currPosition)));
            if(moveDirection == MoveDirection.Up)
                skipNextOnRoadFlag=true;
        }
        else if(currPosValue + currNeighbourhood.currRightMid() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetRightMid(currPosition)));
            if(moveDirection == MoveDirection.Right)
                skipNextOnRoadFlag=true;
        }
        else if(currPosValue + currNeighbourhood.currLeftMid() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetLeftMid(currPosition)));
            if(moveDirection == MoveDirection.Left)
                skipNextOnRoadFlag=true;
        }
        // obwod wokol srodka
        else if(currNeighbourhood.currLeftLower() + currNeighbourhood.currLeftMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetLeftLower(currPosition), PositionHelper.GetLeftMid(currPosition)));
        }
        if(currNeighbourhood.currLeftMid() + currNeighbourhood.currLeftUpper() == valueToFind){
            result.add(new Pair(PositionHelper.GetLeftMid(currPosition), PositionHelper.GetLeftUpper(currPosition)));
        }
        else if(currNeighbourhood.currLeftUpper() + currNeighbourhood.currHighMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetLeftUpper(currPosition), PositionHelper.GetHighMid(currPosition)));
        }
        if(currNeighbourhood.currHighMid() + currNeighbourhood.currRightUpper() == valueToFind){
            result.add(new Pair(PositionHelper.GetHighMid(currPosition), PositionHelper.GetRightUpper(currPosition)));
        }
        else if(currNeighbourhood.currRightUpper() + currNeighbourhood.currRightMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetRightUpper(currPosition), PositionHelper.GetRightMid(currPosition)));
        }
        if(currNeighbourhood.currRightMid() + currNeighbourhood.currRightLower() == valueToFind){
            result.add(new Pair(PositionHelper.GetRightMid(currPosition), PositionHelper.GetRightLower(currPosition)));
        }
        else if(currNeighbourhood.currRightLower() + currNeighbourhood.currLowMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetRightLower(currPosition), PositionHelper.GetLowMid(currPosition)));
        }
        if(currNeighbourhood.currLowMid() + currNeighbourhood.currLeftLower() == valueToFind){
            result.add(new Pair(PositionHelper.GetLowMid(currPosition), PositionHelper.GetLeftLower(currPosition)));
        }
        return result;
    }
}
