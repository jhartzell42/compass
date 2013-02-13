needle 2007;

"Ranges of integers"
"package compass::collection::range;"

private mixin $Range;
$Range mixin $OrderedCollection;
$Range mixin $Immutable;

"TODO [Jimmy] How do we tell the compiler this is immutable?"
private class $SingleRange
    !start,
    !end,
;
$SingleRange mixin $Range;
private class $StepRange ($SingleRange)
    !step,
;
$StepRange mixin $Range;

private $SingleRange constructor newFrom: !start to: !end;
public $Integer ... @end {
    ^$SingleRange newFrom: self to: @end
}

private $StepRange constructor newFrom: @start to: @end step: !step {
    $InvalidArgumentException assert: !step != 0;
    newFrom: @start to: @start+(@end-@start)\!step*!step
}

$Range step: @step {
    $InvalidArgumentException assert: @step != 0;
    ^$StepRange newFrom: first to: last step: step*@step
}

$SingleRange first (7,ro) {
    ^!start
}
$SingleRange last (7,ro) {
    ^!end
}

private $Range abstract #step;
$SingleRange step {
    ^1
}
$StepRange step (7,ro) {
    ^!step
}

$Range size {
    ^(last - first)\step + 1
}
$Range [ @index ] {
    checkIndex: @index;
    ^first + @index*step
}

$Range forwardsLoop: @body {
    @i := first;
    @index := 0;
    { (@i > last) not } -> {
	@body(@i,@index);
	@i := @i + step;
	@index := @index + 1
    }
}

$SingleRange forwardsLoop: @body {
    @i := first;
    @index := 0;
    { @i <= last } -> {
	@body(@i,@index);
	@i := @i + 1;
	@index := @index + 1
    }
}

private $Range indexOf: @elem {
    ^(@elem - first)\step
}

"TODO: make this DRYer"
$Range backwardsLoop: @body {
    @i := last;
    @index := size - 1;
    { (@i basicLess: first) not } -> {
	@body(@i,@index);
	@i := @i basicMinus: step;
	@index := @index - 1
    }
}

$SingleRange contains: @val {
    ^@val between: first and: last
}

$Range contains: @val {
    ^@val in: first...last && {(@val-first) divides: step}
}

$Range species {
    ^$ArrayList
}

$Range addAll: _ {
    $MessageNotUnderstoodException();
}

$Range adding: _ {
    $MessageNotUnderstoodException();
}

public $SingleRange singleRangeIntersect: @otherRange {
    ^(@otherRange first min: first)...(@otherRange last min: last)
}

$Range [ @start ~ @end ~ @skip ] {
    ^[@start]...[@end] step: @skip*step
}

$Range asString {
    ^first asString . '...' . last asString . ' step: ' . step asString
}
