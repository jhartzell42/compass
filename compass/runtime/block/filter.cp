needle 2007;

"TODO doc"
"package compass::block::filter;"

"TODO worry about implementing this package after we have collections"
"TODO filter ideas: reverse args, curry last, ..."
"TODO different types of argument ``filters''. Make a class for
    handling them."
"**
 * Create a new block same as me with my first argument frozen to the
 * specified value. The new block takes one less argument, starting with my
 * second argument.
 *
 * Here is an example: <code>@adder := {@a, @b -- @a - @b}</code>.
 * @adder takes two arguments and substracts them.
 * <code>@foo := @adder curry: 100</code>. Now @foo takes one argument,
 * which we called @b, and returns how far below 100 it is.
 * So, <code>@foo(10)</code> is 90 because it evaluates <code>100-90</code>.
 *
 * TODO does the ``saved'' or something like it make sense here?
 * @reciever[7,ro saved] old block
 * @param[user,saved] value to curry into block
 * @return[7,ro only] new block taking one less argument
 *"
public $Block curryLeft: @arg {
    ^$LeftCurryFilter newWithBlock: self arg: @arg "TODO naming?"
}
"TODO left/right or first/last or both?"
public $Block curryRight: @arg {
    ^@RightCurryFilter newWithBlock: self arg: @arg "TODO naming?"
}
public alias #curry: #curryLeft:;
public alias #<-- #curry:;
"friend compass::block::filter!!???" "TODO implementation goes there"

" Actually, this should be public... "
private class $BlockFilter
    !block;
private $BlockFilter constructor newWithBlock: !block (7,ro);
$BlockFilter mixin $Block;

" Name? "
private $BlockFilter filterArgs: @args {
    ^@args
}

$BlockFilter evaluateWith: @args (7,ro) {
    !block(filterArgs: @args)
}

private class $CurryFilter ($BlockFilter)
    !arg;
private $CurryFilter constructor newWithBlock: @block arg: !arg {
    newWithBlock: @block
}
private $CurryFilter curriedArgument (7,ro) {
    ^!arg
}
private class $LeftCurryFilter ($CurryFilter);
$LeftCurryFilter filterArgs: @args {
    ^[curriedArgument].@args
}
private class $RightCurryFilter ($CurryFilter);
$RightCurryFilter filterArgs: @args {
    ^@args.[curriedArgument]
}
private class $ReverseFilter ($BlockFilter);
$ReverseFilter filterArgs: @args {
    ^@args reverse "TODO declare/implement reverse"
}
