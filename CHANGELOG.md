# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

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
