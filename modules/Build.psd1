@{
    # Script module or binary module file associated with this manifest.
    RootModule        = 'Build.psm1'

    # Version number of this module.
    ModuleVersion     = '1.0'

    # ID used to uniquely identify this module
    GUID              = '1420ce0e-d867-46fe-801a-868414aafccc'

    # Author of this module
    Author            = 'J-Afzal'

    # Company or vendor of this module
    CompanyName       = 'Unknown'

    # Copyright statement for this module
    Copyright         = '(c) 2025 J-Afzal. All rights reserved.'

    # Description of the functionality provided by this module
    Description       = 'Contains functions to build Lane and Object Detection code.'

    # Functions to export from this module, for best performance, do not use wildcards and do not delete the entry, use an empty array if there are no functions to export.
    FunctionsToExport = @(
        'Build-CppCodeUsingCMake'
    )

    # Cmdlets to export from this module, for best performance, do not use wildcards and do not delete the entry, use an empty array if there are no cmdlets to export.
    CmdletsToExport   = @()

    # Aliases to export from this module, for best performance, do not use wildcards and do not delete the entry, use an empty array if there are no aliases to export.
    AliasesToExport   = @()
}
