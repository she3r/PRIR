import java.util.Vector;

public class PositionHelper{
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
    public static CellNeighbourhood GetCurrNeighbourhood(Position2D newPosition, LockingTable2D table,
                                                         boolean canAcquireOnRight, ExploringMode leftExploringMode, MoveDirection moveDirection) throws InterruptedException {
        int col = newPosition.col();
        int row = newPosition.row();
        Integer currLeftUpper = null, currRightUpper = null, currLeftLower = null, currRightLower = null, currLowMid = null,
                currHighMid = null, currRightMid = null, currLeftMid = null;


        if (row == 0) {
            if (col == 0) {
                currHighMid = table.GetValue(PositionHelper.GetHighMid(newPosition));
                switch (moveDirection) {
                    case Up -> {
                        if (canAcquireOnRight) {
                            currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                            currRightUpper = table.AcquireNotifyValue(PositionHelper.GetRightUpper(newPosition));
                        } else {
                            currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                            currRightUpper = table.GetValue(PositionHelper.GetRightUpper(newPosition));
                        }
                    }
                    case Down -> throw new IllegalArgumentException("can only go up");
                }
            } else {
                switch (leftExploringMode) {
                    case Acquire -> {
                        switch (moveDirection) {
                            case Up -> {
                                currLeftMid = table.AcquireValue(PositionHelper.GetLeftMid(newPosition));
                                currLeftUpper = table.AcquireValue(PositionHelper.GetLeftUpper(newPosition));
                            }
                            case Down -> {
                                currLeftUpper = table.AcquireValue(PositionHelper.GetLeftUpper(newPosition));
                                currLeftMid = table.AcquireValue(PositionHelper.GetLeftMid(newPosition));
                            }
                        }
                    }
                    case GetWait -> {
                        currLeftMid = table.GetWaitValue(PositionHelper.GetLeftMid(newPosition));
                        currLeftUpper = table.GetWaitValue(PositionHelper.GetLeftUpper(newPosition));
                    }
                }
                currHighMid = table.GetValue(PositionHelper.GetHighMid(newPosition));
                if (col < table.cols - 1)
                    switch (moveDirection) {
                        case Up -> {
                            if (canAcquireOnRight) {
                                currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                                currRightUpper = table.AcquireNotifyValue(PositionHelper.GetRightUpper(newPosition));
                            } else {
                                currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                                currRightUpper = table.GetValue(PositionHelper.GetRightUpper(newPosition));
                            }
                        }
                        case Down -> throw new IllegalArgumentException("can only go up");
                    }
            }
        } else if (row == table.rows) {
            if (col == 0) {
                currLowMid = table.GetValue(PositionHelper.GetLowMid(newPosition));
                switch (moveDirection) {
                    case Down -> {
                        if (canAcquireOnRight) {
                            currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                            currRightLower = table.AcquireNotifyValue(PositionHelper.GetRightLower(newPosition));
                        } else {
                            currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                            currRightLower = table.GetValue(PositionHelper.GetRightLower(newPosition));
                        }
                    }
                    case Up -> throw new IllegalArgumentException("can only go down");
                }
            } else {
                switch (leftExploringMode){
                    case Acquire -> {
                        switch (moveDirection) {
                            case Down -> {
                                currLeftMid = table.AcquireValue(PositionHelper.GetLeftMid(newPosition));
                                currLeftLower = table.AcquireValue(PositionHelper.GetLeftLower(newPosition));
                            }
                            case Up -> {
                                currLeftLower = table.AcquireValue(PositionHelper.GetLeftLower(newPosition));
                                currLeftMid = table.AcquireValue(PositionHelper.GetLeftMid(newPosition));
                            }
                        }
                    }
                    case GetWait -> {
                        currLeftMid = table.GetWaitValue(PositionHelper.GetLeftMid(newPosition));
                        currLeftLower = table.GetWaitValue(PositionHelper.GetLeftLower(newPosition));
                    }
                }
                currLowMid = table.GetValue(PositionHelper.GetLowMid(newPosition));
                if (col < table.cols - 1)
                    switch (moveDirection) {
                        case Down -> {
                            if (canAcquireOnRight) {
                                currRightLower = table.AcquireNotifyValue(PositionHelper.GetRightLower(newPosition));
                                currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                            } else {
                                currRightLower = table.GetValue(PositionHelper.GetRightLower(newPosition));
                                currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                            }
                        }
                        case Up -> throw new IllegalArgumentException("can only go down");
                    }
            }
        } else {
            if (col == 0) {
                currHighMid = table.GetValue(PositionHelper.GetHighMid(newPosition));
                currLowMid = table.GetValue(PositionHelper.GetLowMid(newPosition));
                if (canAcquireOnRight) {
                    switch (moveDirection) {
                        case Down -> {
                            currRightUpper = table.AcquireNotifyValue(PositionHelper.GetRightUpper(newPosition));
                            currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                            currRightLower = table.AcquireNotifyValue(PositionHelper.GetRightLower(newPosition));
                        }
                        case Up -> {
                            currRightLower = table.AcquireNotifyValue(PositionHelper.GetRightLower(newPosition));
                            currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                            currRightUpper = table.AcquireNotifyValue(PositionHelper.GetRightUpper(newPosition));
                        }
                    }
                } else
                    switch (moveDirection) {
                        case Down -> {
                            currRightUpper = table.GetValue(PositionHelper.GetRightUpper(newPosition));
                            currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                            currRightLower = table.GetValue(PositionHelper.GetRightLower(newPosition));
                        }
                        case Up -> {
                            currRightLower = table.GetValue(PositionHelper.GetRightLower(newPosition));
                            currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                            currRightUpper = table.GetValue(PositionHelper.GetRightUpper(newPosition));
                        }
                    }
            } else {
                switch (leftExploringMode){
                    case Acquire -> {
                        switch (moveDirection) {
                            case Down -> {
                                currLeftUpper = table.GetValue(PositionHelper.GetLeftUpper(newPosition));
                                currLeftMid = table.GetValue(PositionHelper.GetLeftMid(newPosition));
                                currLeftLower = table.GetValue(PositionHelper.GetLeftLower(newPosition));
                            }
                            case Up -> {
                                currLeftLower = table.GetValue(PositionHelper.GetLeftLower(newPosition));
                                currLeftMid = table.GetValue(PositionHelper.GetLeftMid(newPosition));
                                currLeftUpper = table.GetValue(PositionHelper.GetLeftUpper(newPosition));
                            }
                        }

                    }
                    case GetWait -> {
                        currLeftUpper = table.GetValue(PositionHelper.GetLeftUpper(newPosition));
                        currLeftMid = table.GetValue(PositionHelper.GetLeftMid(newPosition));
                        currLeftLower = table.GetValue(PositionHelper.GetLeftLower(newPosition));
                    }
                }

                currHighMid = table.GetValue(PositionHelper.GetHighMid(newPosition));
                currLowMid = table.GetValue(PositionHelper.GetLowMid(newPosition));
                if (col < table.cols - 1) {
                    if (canAcquireOnRight) {
                        switch (moveDirection) {
                            case Down -> {
                                currRightUpper = table.AcquireNotifyValue(PositionHelper.GetRightUpper(newPosition));
                                currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                                currRightLower = table.AcquireNotifyValue(PositionHelper.GetRightLower(newPosition));
                            }
                            case Up -> {
                                currRightLower = table.AcquireNotifyValue(PositionHelper.GetRightLower(newPosition));
                                currRightMid = table.AcquireNotifyValue(PositionHelper.GetRightMid(newPosition));
                                currRightUpper = table.AcquireNotifyValue(PositionHelper.GetRightUpper(newPosition));
                            }
                        }
                    } else
                        switch (moveDirection) {
                            case Down -> {
                                currRightUpper = table.GetValue(PositionHelper.GetRightUpper(newPosition));
                                currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                                currRightLower = table.GetValue(PositionHelper.GetRightLower(newPosition));
                            }
                            case Up -> {
                                currRightLower = table.GetValue(PositionHelper.GetRightLower(newPosition));
                                currRightMid = table.GetValue(PositionHelper.GetRightMid(newPosition));
                                currRightUpper = table.GetValue(PositionHelper.GetRightUpper(newPosition));
                            }
                        }
                }
            }
        }
            return new CellNeighbourhood(currLeftUpper, currRightUpper, currLeftLower, currRightLower, currLowMid, currHighMid, currRightMid, currLeftMid);
        }

    // zakladamy brak kolizji
    public static Vector<Pair> FindPairs(LockingTable2D table, Position2D currPosition,
                                 CellNeighbourhood currNeighbourhood, int valueToFind){
        Vector<Pair> result = new Vector<>();
        int currPosValue = table.AcquireNotifyValue(currPosition); // pobierz pozycje stacjonarna
        // ze srodka
        if(currNeighbourhood.currLeftUpper() != null && currPosValue + currNeighbourhood.currLeftUpper() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetLeftUpper(currPosition)));
        }
        else if(currNeighbourhood.currRightUpper() != null && currPosValue + currNeighbourhood.currRightUpper() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetRightUpper(currPosition)));
        }
        else if(currNeighbourhood.currLeftLower() != null && currPosValue + currNeighbourhood.currLeftLower() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetLeftLower(currPosition)));
        }
        else if(currNeighbourhood.currRightLower() != null && currPosValue + currNeighbourhood.currRightLower() == valueToFind){
            result.add(new Pair(currPosition, PositionHelper.GetRightLower(currPosition)));
        }
        // obwod wokol srodka
        else if(currNeighbourhood.currLeftLower() != null && currNeighbourhood.currLeftMid() != null &&
                currNeighbourhood.currLeftLower() + currNeighbourhood.currLeftMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetLeftLower(currPosition), PositionHelper.GetLeftMid(currPosition)));
        }
        if(currNeighbourhood.currLeftMid() != null && currNeighbourhood.currLeftUpper() != null &&
                currNeighbourhood.currLeftMid() + currNeighbourhood.currLeftUpper() == valueToFind){
            result.add(new Pair(PositionHelper.GetLeftMid(currPosition), PositionHelper.GetLeftUpper(currPosition)));
        }
        else if(currNeighbourhood.currLeftUpper() != null && currNeighbourhood.currHighMid() != null &&
                currNeighbourhood.currLeftUpper() + currNeighbourhood.currHighMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetLeftUpper(currPosition), PositionHelper.GetHighMid(currPosition)));
        }
        if(currNeighbourhood.currHighMid() != null && currNeighbourhood.currRightUpper() != null &&
                currNeighbourhood.currHighMid() + currNeighbourhood.currRightUpper() == valueToFind){
            result.add(new Pair(PositionHelper.GetHighMid(currPosition), PositionHelper.GetRightUpper(currPosition)));
        }
        else if(currNeighbourhood.currRightUpper() != null && currNeighbourhood.currRightMid() != null &&
                currNeighbourhood.currRightUpper() + currNeighbourhood.currRightMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetRightUpper(currPosition), PositionHelper.GetRightMid(currPosition)));
        }
        if(currNeighbourhood.currRightMid() != null && currNeighbourhood.currRightLower() != null &&
                currNeighbourhood.currRightMid() + currNeighbourhood.currRightLower() == valueToFind){
            result.add(new Pair(PositionHelper.GetRightMid(currPosition), PositionHelper.GetRightLower(currPosition)));
        }
        else if(currNeighbourhood.currRightLower() != null && currNeighbourhood.currLowMid() != null &&
                currNeighbourhood.currRightLower() + currNeighbourhood.currLowMid() == valueToFind){
            result.add(new Pair(PositionHelper.GetRightLower(currPosition), PositionHelper.GetLowMid(currPosition)));
        }
        if(currNeighbourhood.currLowMid() != null && currNeighbourhood.currLeftLower() != null &&
                currNeighbourhood.currLowMid() + currNeighbourhood.currLeftLower() == valueToFind){
            result.add(new Pair(PositionHelper.GetLowMid(currPosition), PositionHelper.GetLeftLower(currPosition)));
        }
        return result;
    }
}
