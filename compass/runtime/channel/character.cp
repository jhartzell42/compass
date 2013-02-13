needle 2007;

"package compass::channel::character;"
public mixin $CharacterOutputChannel;
$CharacterOutputChannel mixin $OutputChannel;

public $CharacterOutputChannel print: @printable {
    printString: @printable asString
}

public $CharacterOutputChannel printLine: @printable {
    print: @printable;
    printNewLine
}

public $CharacterOutputChannel printNewLine;

public $CharacterOutputChannel printString: @str {
    @str -> { @char --
	put: @char
    }
}

public mixin $CharacterInputChannel;
$CharacterInputChannel mixin $InputChannel;

public $CharacterInputChannel nextLine {
    @acc := @lastChar := '';
    { @lastChar != '' class newline } -> {
	@acc := @acc . (@lastChar := next);
    };
    ^@acc
}

public $CharacterInputChannel slurp {
    @res := '';
    -> { @chr --
	@res := @res . @chr
    };
    ^@res
}
