needle 2007;

"TODO doc"
"package compass::string;"

public mixin $String;

$String mixin $OrderedCollection;
$String mixin $ComparableSpaceshipAdaptor;
$String mixin $Immutable;

public mixin $Character;
$Character mixin $String;

$Character size {
    ^1
}

public $String isCharacter {
    ^$false
}

$Character isCharacter {
    ^$true
}

$String stringCompare: @other {
    @sharedIndexRange := indexRange singleRangeIntersect: @other indexRange;
    @sharedIndexRange forwardsLoop: { @index --
	@comparison := ([@index] <=> @other[@index]);
	@comparison !== $equal and: { ^@comparison};
    };
    ^size <=> @other size
}

public $String alias #<=> #stringCompare:;

public $String asString;

public $String class abstract #newline;

public $String chomp {
    last = class newline and: {
	^[0 ~ size-2 ~ 1]
    }
}

$Boolean asString {
    ^?{
	'true'
    }//{
	'false'
    }
}
