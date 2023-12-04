# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
- Fixed HF when resetting the world when using DART as PE(https://github.com/icub-tech-iit/ergocub-software/issues/190)

## [0.6.0] - 2023-11-15

### urdf
- Fixed location of `l/r_sole` frames(https://github.com/icub-tech-iit/ergocub-software/issues/186)
- Fixed alignment of depth to the RGB frame(https://github.com/icub-tech-iit/ergocub-software/issues/183)

## [0.5.0] - 2023-10-26

### urdf
- Fixed wobbling of the models after moving feet link frames

## [0.4.0] - 2023-09-04

### urdf
- Added ergoCubGazeboV1_1/_minContacs ergoCubSN001
- Repository refactored for using creo2urdf

## [0.3.4] - 2023-08-28

### urdf
- ergoCubGazeboV1: reenabled `torso_pitch` (https://github.com/icub-tech-iit/ergocub-software/issues/152)
- ergoCubGazeboV1: "center" feet csys w/ ft csys (https://github.com/icub-tech-iit/ergocub-software/issues/146)

## [0.3.3] - 2023-07-07

- Fixed concurrency in `ergoCubEmotions`(https://github.com/icub-tech-iit/ergocub-software/issues/138)

## [0.3.2] - 2023-06-16

### urdf
- Fixed lidar frame (https://github.com/icub-tech-iit/ergocub-software/pull/134)

## [0.3.0] - 2023-06-05

### urdf

- Changed the way the FT are published, now are divided per parts (https://github.com/icub-tech-iit/ergocub-software/pull/123)
- Added imu in the waist (https://github.com/icub-tech-iit/ergocub-software/pull/99)
- Locked `torso_pitch` to 10 deg (https://github.com/icub-tech-iit/ergocub-software/issues/102)
- Fixed pinkie taxels exportation (https://github.com/icub-tech-iit/ergocub-software/issues/79)
- Added the model `ergoCubGazeboV1_minContacts`

### ergoCubEmotions
- Added first draft of `ergoCubEmotions`

## [0.2.0] - 2023-03-07

### urdf
- Removed hip ft sensors from `ergoCubSN000` model.
- Aligned joint names of the hands with the real robot.
- Fixed left arm small inertia problems (https://github.com/icub-tech-iit/ergocub-software/issues/60)

## [0.1.0] - 2023-01-24

### urdf
- Add first complete version of urdf of `ergoCub`.
