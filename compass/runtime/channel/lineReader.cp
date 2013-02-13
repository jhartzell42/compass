needle 2007;

public class $LineReader
    !characterChannel,
;

$LineReader next (7,rw) {
    ^!characterChannel nextLine
}

public $LineReader constructor newFromChannel: !characterChannel;

public $CharacterInputChannel lineReader {
    ^wrap: [$LineReader,$BufferedInputChannel,]
}
