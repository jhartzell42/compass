"* throws none
 * prints 207
 * prints 39
 *"
needle 2007;

"** Tests super. *"
"package test::super;"
public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @out := out;
    @test := $IVarTesterChild new;
    @test { printTo: @out; change; printTo: @out; };
}

private class $IVarTester !info;
private class $IVarTesterChild ($IVarTester) !info;

private $IVarTester constructor new {
    !info := $false
}
private $IVarTesterChild constructor new {
    super new;
    !info := $true
}

private $IVarTester change {
    !info := !info not
}
private $IVarTesterChild change {
    super change;
    !info := !info not
}

private $IVarTester read {
    ^!info
}

private $IVarTesterChild printTo: @out {
    @out printLine: (read?{39}//{207})
}
