needle 2007;
"package compass:string::ascii;"
public class $AsciiString
    !size,
    "special"
;

$AsciiString mixin $String;

public class $AsciiCharacter ($AsciiString);
$AsciiCharacter mixin $Character;

private $AsciiCharacter class #newWithCodePoint: #MakeAsciiCharacter;

$AsciiString size (7,ro) {
    ^!size
}

$AsciiString [ @index ] (7,ro) {
    ^[@index~@index~1]
}

$AsciiString #primitiveSliceFrom:to:skip:onBounds: #SliceAsciiString;

$AsciiString [@a~@b~@c] {
    ^primitiveSliceFrom: @a to: @b skip: @c onBounds: {
	$IndexOutOfBoundsException()
    }
}

$AsciiString species {
    ^$AsciiString
}

$AsciiString class new {
    ^''
}

private $AsciiString assertIsAsciiString;
$Object assertIsAsciiString {
    $InvalidArgumentException()
}
$AsciiString . @otherString (7,ro) {
    @otherString := @otherString asString;
    @otherString assertIsAsciiString;
    ^primitiveConcat: @otherString 
}

$AsciiString #primitiveConcat: #StringConcat;

private $AsciiCharacter codePoint {
    ^primitiveGet: 0 
}

$AsciiString #primitiveGet: #StringGet;

private $AsciiCharacter class createDigit: @num {
    @num < 10 ? {
	^newWithCodePoint: 48 + @num
    } // {
	^newWithCodePoint: 65 + @num - 10
    }
}

private $AsciiString class newline {
    ^$AsciiCharacter newWithCodePoint: 10
}

private $AsciiString class tab {
    ^$AsciiCharacter newWithCodePoint: 9
}

private $AsciiString #compareTo:withLessValue:greaterValue:equalValue:
			#StringCompare;

private $AsciiString <=> @other {
    @other assertIsAsciiString;
    ^compareTo: @other
	    withLessValue: $less
		greaterValue: $greater
		equalValue: $equal
}
