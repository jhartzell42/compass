"* throws none
 * prints 207
 * prints 39
 *"
needle 2007;

"** Tests that a boolean can be remembered in an ivar. *"
"package test::ivars;"

public class $Test ($ConsoleProgram);

$Test run (7,rw) {
    @out := out;
    @test := $IVarTester new;
    @test { printTo: @out; change; printTo: @out; };
}

private class $IVarTester !info;

private $IVarTester constructor new {
    !info := $false
}

private $IVarTester change {
    !info := !info not
}

private $IVarTester printTo: @out {
    @out printLine: (!info?{39}//{207})
}
