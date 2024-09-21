$cd "C:\Users\DED_MAKSIM_116_rus\Desktop\1\ct_gign"
$modelname "player/ct_gign.mdl"
$model "ct_gign" "ct_gign_reference.smd"
$lod 10
 {
  replacemodel "ct_gign_reference" "lod1_ct_gign_reference"
}
$lod 20
 {
  replacemodel "ct_gign_reference" "lod2_ct_gign_reference"
}
$lod 35
 {
  replacemodel "ct_gign_reference" "lod3_ct_gign_reference"
}
$lod 50
 {
  replacemodel "ct_gign_reference" "lod4_ct_gign_reference"
}
$lod 70
 {
  replacemodel "ct_gign_reference" "lod5_ct_gign_reference"
}
$shadowlod
{
  replacemodel "ct_gign_reference" "lod6_ct_gign_reference"
}
$poseparameter move_yaw -180.00 180.00 360.00
$poseparameter body_pitch -90.00 90.00 360.00
$poseparameter body_yaw -90.00 90.00 360.00
$cdmaterials "models\player\ct_gign\"
$hboxset "cstrike"
$hbox 3 "ValveBiped.Bip01_Pelvis" -8.299  -7.727  -5.429  8.299  2.007  6.583
$hbox 6 "ValveBiped.Bip01_L_Thigh" 4.576  -5.377  -3.432  22.880  3.775  4.576
$hbox 6 "ValveBiped.Bip01_L_Calf" -0.291  -4.295  -3.432  20.883  3.151  3.432
$hbox 6 "ValveBiped.Bip01_L_Foot" -2.288  -1.144  -2.579  5.720  4.576  2.007
$hbox 6 "ValveBiped.Bip01_L_Toe0" -2.860  -2.808  -2.860  4.004  1.196  1.716
$hbox 7 "ValveBiped.Bip01_R_Thigh" 4.576  -5.377  -3.432  22.880  3.775  4.576
$hbox 7 "ValveBiped.Bip01_R_Calf" -0.291  -4.295  -3.432  20.883  3.151  3.432
$hbox 7 "ValveBiped.Bip01_R_Foot" -2.288  -1.144  -2.007  5.720  4.576  2.579
$hbox 7 "ValveBiped.Bip01_R_Toe0" -2.860  -2.808  -1.716  4.004  1.196  2.860
$hbox 3 "ValveBiped.Bip01_Spine1" -9.152  -1.258  -8.008  4.576  11.326  8.008
$hbox 2 "ValveBiped.Bip01_Spine2" -3.068  -3.432  -9.693  13.572  10.317  9.693
$hbox 1 "ValveBiped.Bip01_Neck1" -0.104  -4.576  -2.860  5.512  2.288  2.860
$hbox 1 "ValveBiped.Bip01_Head1" -0.416  -6.240  -3.640  9.360  4.441  3.016
$hbox 4 "ValveBiped.Bip01_L_UpperArm" -1.144  -2.579  -2.288  14.872  2.579  2.288
$hbox 4 "ValveBiped.Bip01_L_Forearm" -1.716  -2.402  -2.402  13.156  2.402  2.402
$hbox 4 "ValveBiped.Bip01_L_Hand" 0.291  -2.465  -1.716  6.583  1.550  2.860
$hbox 5 "ValveBiped.Bip01_R_UpperArm" -1.144  -2.579  -2.288  14.872  2.579  2.288
$hbox 5 "ValveBiped.Bip01_R_Forearm" -1.716  -2.402  -2.402  13.156  2.402  2.402
$hbox 5 "ValveBiped.Bip01_R_Hand" 0.291  -2.465  -2.860  6.583  1.550  1.716
// Model uses material "ct_gign.vmt"
// Model uses material "ct_gign_glass.vmt"
$attachment "forward" "ValveBiped.forward" 0.00 -0.00 0.00 rotate 0.00 0.00 -0.00
$attachment "grenade0" "ValveBiped.Bip01_Spine" 5.99 0.39 7.59 rotate 76.91 71.22 57.14
$attachment "grenade1" "ValveBiped.Bip01_Spine" 6.24 -2.34 6.66 rotate 62.97 100.42 87.46
$attachment "grenade2" "ValveBiped.Bip01_Spine" -0.35 2.21 -8.34 rotate -75.89 92.68 95.17
$attachment "grenade3" "ValveBiped.Bip01_Spine" 1.22 -1.11 -7.34 rotate -78.22 82.38 102.13
$attachment "pistol" "ValveBiped.Bip01_R_Thigh" 0.05 3.84 -6.30 rotate 0.08 -2.59 79.96
$attachment "primary" "ValveBiped.Bip01_Spine2" 10.44 -5.10 -4.88 rotate -11.71 179.79 3.52
$attachment "defusekit" "ValveBiped.Bip01_Pelvis" 0.00 1.22 -6.66 rotate -73.06 90.93 179.75
$attachment "eholster" "ValveBiped.Bip01_L_Thigh" 5.36 0.02 4.90 rotate 8.31 -91.56 99.67
$attachment "lfoot" "ValveBiped.Bip01_L_Foot" 0.00 5.20 0.00 rotate -0.00 0.00 -0.00
$attachment "rfoot" "ValveBiped.Bip01_R_Foot" -0.00 5.20 0.00 rotate 0.00 -0.00 0.00
$attachment "muzzle_flash" "ValveBiped.Bip01_L_Hand" -0.00 -0.00 0.00 rotate -0.00 -0.00 0.00
$includemodel "player/cs_player_shared.mdl"
$includemodel "player/ak_anims_ct.mdl"
$surfaceprop "flesh"
$eyeposition -0.000 0.000 73.000
$illumposition 0.305 0.019 37.327
$sequence ragdoll "ragdoll" ACT_DIERAGDOLL 1 fps 30.00
$ikchain rhand ValveBiped.Bip01_R_Hand knee  0.707 0.707 0.000
$ikchain lhand ValveBiped.Bip01_L_Hand knee  0.707 0.707 0.000
$ikchain rfoot ValveBiped.Bip01_R_Foot knee  0.707 -0.707 0.000
$ikchain lfoot ValveBiped.Bip01_L_Foot knee  0.707 -0.707 0.000
$ikchain ikclip ValveBiped.weapon_bone_Clip knee  0.707 -0.707 0.000
$collisionjoints "phymodel.smd" {

	$mass 100.0
	$inertia 10.00
	$damping 0.05
	$rotdamping 5.00
	$rootbone "valvebiped.bip01_pelvis"
	$jointrotdamping "valvebiped.bip01_pelvis" 3.00

	$jointmassbias "valvebiped.bip01_spine1" 8.00
	$jointconstrain "valvebiped.bip01_spine1" x limit -10.00 10.00 0.00
	$jointconstrain "valvebiped.bip01_spine1" y limit -16.00 16.00 0.00
	$jointconstrain "valvebiped.bip01_spine1" z limit -20.00 30.00 0.00

	$jointmassbias "valvebiped.bip01_spine2" 9.00
	$jointconstrain "valvebiped.bip01_spine2" x limit -10.00 10.00 0.00
	$jointconstrain "valvebiped.bip01_spine2" y limit -10.00 10.00 0.00
	$jointconstrain "valvebiped.bip01_spine2" z limit -20.00 20.00 0.00

	$jointmassbias "valvebiped.bip01_r_clavicle" 4.00
	$jointrotdamping "valvebiped.bip01_r_clavicle" 6.00
	$jointconstrain "valvebiped.bip01_r_clavicle" x limit -15.00 15.00 0.00
	$jointconstrain "valvebiped.bip01_r_clavicle" y limit -10.00 10.00 0.00
	$jointconstrain "valvebiped.bip01_r_clavicle" z limit 0.00 45.00 0.00

	$jointmassbias "valvebiped.bip01_l_clavicle" 4.00
	$jointrotdamping "valvebiped.bip01_l_clavicle" 6.00
	$jointconstrain "valvebiped.bip01_l_clavicle" x limit -15.00 15.00 0.00
	$jointconstrain "valvebiped.bip01_l_clavicle" y limit -10.00 10.00 0.00
	$jointconstrain "valvebiped.bip01_l_clavicle" z limit 0.00 45.00 0.00

	$jointmassbias "valvebiped.bip01_l_upperarm" 5.00
	$jointrotdamping "valvebiped.bip01_l_upperarm" 2.00
	$jointconstrain "valvebiped.bip01_l_upperarm" x limit -15.00 20.00 0.00
	$jointconstrain "valvebiped.bip01_l_upperarm" y limit -40.00 32.00 0.00
	$jointconstrain "valvebiped.bip01_l_upperarm" z limit -80.00 25.00 0.00

	$jointmassbias "valvebiped.bip01_l_forearm" 4.00
	$jointrotdamping "valvebiped.bip01_l_forearm" 4.00
	$jointconstrain "valvebiped.bip01_l_forearm" x limit -40.00 15.00 0.00
	$jointconstrain "valvebiped.bip01_l_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_l_forearm" z limit -120.00 10.00 0.00

	$jointrotdamping "valvebiped.bip01_l_hand" 1.00
	$jointconstrain "valvebiped.bip01_l_hand" x limit -25.00 25.00 0.00
	$jointconstrain "valvebiped.bip01_l_hand" y limit -35.00 35.00 0.00
	$jointconstrain "valvebiped.bip01_l_hand" z limit -50.00 50.00 0.00

	$jointmassbias "valvebiped.bip01_r_upperarm" 5.00
	$jointrotdamping "valvebiped.bip01_r_upperarm" 2.00
	$jointconstrain "valvebiped.bip01_r_upperarm" x limit -15.00 20.00 0.00
	$jointconstrain "valvebiped.bip01_r_upperarm" y limit -40.00 32.00 0.00
	$jointconstrain "valvebiped.bip01_r_upperarm" z limit -80.00 25.00 0.00

	$jointmassbias "valvebiped.bip01_r_forearm" 4.00
	$jointrotdamping "valvebiped.bip01_r_forearm" 4.00
	$jointconstrain "valvebiped.bip01_r_forearm" x limit -40.00 15.00 0.00
	$jointconstrain "valvebiped.bip01_r_forearm" y limit 0.00 0.00 0.00
	$jointconstrain "valvebiped.bip01_r_forearm" z limit -120.00 10.00 0.00

	$jointrotdamping "valvebiped.bip01_r_hand" 1.00
	$jointconstrain "valvebiped.bip01_r_hand" x limit -25.00 25.00 0.00
	$jointconstrain "valvebiped.bip01_r_hand" y limit -35.00 35.00 0.00
	$jointconstrain "valvebiped.bip01_r_hand" z limit -50.00 50.00 0.00

	$jointmassbias "valvebiped.bip01_r_thigh" 7.00
	$jointrotdamping "valvebiped.bip01_r_thigh" 7.00
	$jointconstrain "valvebiped.bip01_r_thigh" x limit -25.00 25.00 0.00
	$jointconstrain "valvebiped.bip01_r_thigh" y limit -10.00 15.00 0.00
	$jointconstrain "valvebiped.bip01_r_thigh" z limit -55.00 25.00 0.00

	$jointmassbias "valvebiped.bip01_r_calf" 4.00
	$jointconstrain "valvebiped.bip01_r_calf" x limit -10.00 25.00 0.00
	$jointconstrain "valvebiped.bip01_r_calf" y limit -5.00 5.00 0.00
	$jointconstrain "valvebiped.bip01_r_calf" z limit -10.00 115.00 0.00

	$jointmassbias "valvebiped.bip01_l_thigh" 7.00
	$jointrotdamping "valvebiped.bip01_l_thigh" 7.00
	$jointconstrain "valvebiped.bip01_l_thigh" x limit -25.00 25.00 0.00
	$jointconstrain "valvebiped.bip01_l_thigh" y limit -10.00 15.00 0.00
	$jointconstrain "valvebiped.bip01_l_thigh" z limit -55.00 25.00 0.00

	$jointmassbias "valvebiped.bip01_l_calf" 4.00
	$jointconstrain "valvebiped.bip01_l_calf" x limit -10.00 25.00 0.00
	$jointconstrain "valvebiped.bip01_l_calf" y limit -5.00 5.00 0.00
	$jointconstrain "valvebiped.bip01_l_calf" z limit -10.00 115.00 0.00

	$jointmassbias "valvebiped.bip01_head1" 4.00
	$jointrotdamping "valvebiped.bip01_head1" 3.00
	$jointconstrain "valvebiped.bip01_head1" x limit -50.00 50.00 0.00
	$jointconstrain "valvebiped.bip01_head1" y limit -20.00 20.00 0.00
	$jointconstrain "valvebiped.bip01_head1" z limit -26.00 30.00 0.00
}
