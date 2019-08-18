$versionline = Get-Content -Path "build.h"
$splitline = $versionline.split('"')
$versionparts = $splitline.split('.')
$versionparts[4] = [convert]::ToInt32($versionparts[4])+1
$output='#define BUILDVERSION ' + '"'+$versionparts[1]+'.'+$versionparts[2]+'.'+$versionparts[3]+'.'+$versionparts[4]+'"'
echo $output > build.h
