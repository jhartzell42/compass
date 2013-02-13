needle 2007;

public class $ConsoleProgram
    !in,
    !out,
    !err,
;

$ConsoleProgram constructor newWithInput: !in andOutput: !out andError: !err;

public $ConsoleProgram out {
    ^!out
}

public $ConsoleProgram err {
    ^!err
}

public $ConsoleProgram in {
    ^!in
}
