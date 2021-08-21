%define name    wxdfast
%define version 0.70.0

%define is_mageia %(test -e /etc/mageia-release && echo 1 || echo 0)
%define is_suse %(test -e /etc/SuSE-release && echo 1 || echo 0)
%define is_fedora %(test -e /etc/fedora-release && echo 1 || echo 0)

%if %is_fedora
%define distr %(cat /etc/fedora-release)
%endif
%if %is_suse
%define distr %(head -1 /etc/SuSE-release)
%endif
%if %is_mageia
%define distr %(cat /etc/mageia-release)
%endif

Name: %{name}
Version: %{version}
Summary: Multithreaded download manager
Release: 1
License: GPL
Group: Accessories
%if %is_mageia
BuildRequires: wxgtk3.0-devel aria2-devel
%endif
%if %is_fedora
BuildRequires: wxGTK3-devel aria2-devel
%endif
%if %is_suse
BuildRequires: wxWidgets-devel >= 3 aria2-devel
%endif
Source: %{name}-%{version}.tar.gz
Packager: David Vachulka <archdvx@dxsolutions.org>
BuildRoot: %{_tmppath}/%{name}-buildroot
BuildRequires: cmake pkgconfig

%description
Multithreaded download manager. wxDownload Fast is desktop independent and is written with the wxWidgets toolkit and Aria2.

%global debug_package %{nil}

%prep
%setup -q

%build
mkdir build
pushd build
%if %is_fedora
cmake \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    -DDX_WX_CONFIG=wx-config-3.0 \
    ..
%else
cmake \
    -DCMAKE_INSTALL_PREFIX=%{_prefix} \
    ..
%endif
make
popd #build

%install
pushd build
%make_install
popd #build

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%doc COPYING
%{_bindir}/%{name}
%{_bindir}/wxdfast-integrator
%{_datadir}/icons/*
%{_datadir}/applications/%{name}.desktop
%{_datadir}/locale/*
%{_datadir}/wxdfast/icons/*
%{_datadir}/wxdfast/extensionid
