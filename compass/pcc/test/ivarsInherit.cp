"* throws none
 * prints 207
 * prints 39
 *"
needle 2007;

"** Tests that a boolean can be remembered in an ivar of a superclass. *"
"package test::ivars::inheritence;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @out := out;
    @test := $IVarTesterChild new;
    @test { printTo: @out; change; printTo: @out; };
}

private class $IVarTester !info;
private class $IVarTesterChild ($IVarTester);

private $IVarTester constructor new {
    !info := $false
}

private $IVarTester change {
    !info := !info not
}

private $IVarTester read {
    ^!info
}

private $IVarTesterChild printTo: @out {
    @out printLine: (read?{39}//{207})
}
