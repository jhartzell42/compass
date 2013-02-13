"**
 * Contains all basic number methods, and a number mixin which handles
 * type promotion for all of them, and default error-generating stubs.
 *"
"TODO doc"
"package compass::number;"
"**
 * TODO: Dan, what the heck is a number?
 *"
public mixin $Number;

public $Number typeMatches: @other {
    ^species = @other species
}

private exception $PromotionNeededException #onPromotionNeeded:;

public $Number promoteToMatch: @other {
    ^as: @other species
}

public $Number abstract #as:;

private $Number operateSending: @sel withBasicSelector: @basic with: @other {
    ^{
	trySending: @basic with: @other
    } onPromotionNeeded: {
	@sel(promoteToMatch: @other,@other) 
    }
}

private $Number trySending: @sel with: @other {
    $PromotionNeededException assert: (typeMatches: @other);
    ^@sel(self,@other)
}

public $Number + @other {
    ^operateSending: #+ withBasicSelector: (#basicPlus:) with: @other
}

public $Number - @other {
    ^operateSending: #- withBasicSelector: (#basicMinus:) with: @other
}

public $Number * @other {
    ^operateSending: #* withBasicSelector: (#basicTimes:) with: @other
}

public $Number / @other {
    ^operateSending: #/ withBasicSelector: (#basicDivide:) with: @other
}

public $Number % @other {
    ^operateSending: #% withBasicSelector: (#basicMod:) with: @other
}

public $Number abstract #basicPlus:;
public $Number abstract #basicMinus:;
public $Number abstract #basicTimes:;
public $Number abstract #basicDivide:;
public $Number abstract #basicMod:;
public $Number abstract #basicExponentiate:;

public $Number abstract #negate;

$Number mixin $ComparableLessGreaterAdaptor;
$Number < @other {
    ^operateSending: #< withBasicSelector: (#basicLess:) with: @other
}

$Number > @other {
    ^operateSending: #> withBasicSelector: (#basicGreater:) with: @other
}
