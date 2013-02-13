"* throws none
 * prints 207
 * prints 39
 *"
needle 2007;

"** Tests shadowing ivars. *"
"package test::ivars::shadow;"

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    @out := out;
    @test := $IVarTesterChild new;
    @test { printTo: @out; changeBoth; printTo: @out; };
}

private class $IVarTester !info;
private class $IVarTesterChild ($IVarTester) !info;

private $IVarTester setUpInfo {
    !info := $false
}
private $IVarTester constructor new {
    setUpInfo
}
private $IVarTesterChild constructor new {
    setUpInfo;
    !info := $true
}

private $IVarTester change {
    !info := !info not
}
private $IVarTesterChild changeBoth {
    change;
    !info := !info not
}

private $IVarTester read {
    ^!info
}

private $IVarTesterChild printTo: @out {
    @out printLine: (read?{39}//{207})
}
