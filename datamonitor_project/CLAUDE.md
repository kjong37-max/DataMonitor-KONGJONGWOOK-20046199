# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 프로젝트 개요

데이터 모니터링 Tool에 대한 PoC(Proof of Concept) 개발 프로젝트입니다. 데이터 수집·감시·이상 감지 등의 핵심 기능을 검증하기 위한 Windows 콘솔 애플리케이션으로 구현됩니다.

## 빌드 환경

- **IDE:** Visual Studio 2022 (프로젝트 툴셋: v145)
- **언어 표준:** C++20 (`stdcpp20`)
- **대상 플랫폼:** Windows 10 이상 (Win32 / x64)
- **문자셋:** Unicode
- **보안 검사:** SDL Check 활성화

### MSBuild 빌드 명령

```powershell
# Debug x64 빌드
msbuild DataMonitor.slnx /p:Configuration=Debug /p:Platform=x64

# Release x64 빌드
msbuild DataMonitor.slnx /p:Configuration=Release /p:Platform=x64

# 클린 후 빌드
msbuild DataMonitor.slnx /t:Clean,Build /p:Configuration=Debug /p:Platform=x64
```

솔루션 파일(`DataMonitor.slnx`)은 저장소 루트(`C:\reviewer\DataMonitor\`)에 위치합니다. 프로젝트 파일(`datamonitor_project.vcxproj`)은 `datamonitor_project\` 하위에 있습니다.

## 프로젝트 구성

| 구성 | 설명 |
|------|------|
| `Debug\|Win32` | 32비트 디버그, 디버그 정보 포함 |
| `Release\|Win32` | 32비트 릴리스, 전체 프로그램 최적화 |
| `Debug\|x64` | 64비트 디버그, 디버그 정보 포함 |
| `Release\|x64` | 64비트 릴리스, 전체 프로그램 최적화 |

## 현재 상태

소스 파일이 없는 빈 스켈레톤 프로젝트입니다. PoC 개발 착수 전 단계로, `.vcxproj`의 `ItemGroup`에 소스 파일(`.cpp`)과 헤더 파일(`.h`)을 추가하면서 구현을 시작합니다.

소스 파일을 프로젝트에 추가할 때는 `datamonitor_project.vcxproj.filters`의 필터 항목도 함께 업데이트해야 합니다 (소스 파일 → `소스 파일`, 헤더 → `헤더 파일` 필터).
