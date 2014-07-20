Name:           triumph4php
Version:        0.5.1
Release:        1%{?dist}
Summary:        Open source, cross-platform editor and IDE dedicated exclusively for PHP 
License:        MIT
URL:            http://triumph4php.com
Source0:        triumph4php-0.5.1

#BuildRequires: glibc.i686 
BuildRequires: community-mysql-devel
BuildRequires: sqlite-devel
BuildRequires: gtk2-devel 
BuildRequires: libicu-devel
BuildRequires: cmake
BuildRequires: libcurl-devel
BuildRequires: boost-system
BuildRequires: asio-devel

Requires: community-mysql      
Requires: sqlite      
Requires: curl
Requires: gtk2  
Requires: libicu 
Requires: boost-system

%description
Triumph 4 PHP is an IDE where PHP is a first-class citizen. Triumph was 
written exclusively for developing mid to large size web applications 
that run on the LAMP/WAMP stack. It saves you time by allowing you to 
easily navigate through your project's directory structure, database 
schema, and class hierarchy, no matter how deep.

Triumph does all of this in a clutter-free, tight interface that has a 
native look on either Windows or Linux.

Win the battle against PHP web applications by using Triumph 4 PHP!

%prep
cp -r %SOURCE0/* .

%build
#make config=release clean
T4P_BUILDSCRIPTS_DIR=. ./premake4 gmake
make config=release triumph4php  %{?_smp_mflags}

%install
install -d %{buildroot}/%{_bindir}
install -d %{buildroot}/%{_libdir}
install -d %{buildroot}/%{_libdir}/%{name}
install -d %{buildroot}/%{_datadir}
install -d %{buildroot}/%{_datadir}/applications/
cp Release/triumph4php %{buildroot}/%{_bindir}
cp -R Release/*.so*  %{buildroot}/%{_libdir}/%{name}
cp -R assets      %{buildroot}/%{_datadir}/%{name}
cp ~/rpmbuild/SPECS/triumph4php.desktop %{buildroot}/%{_datadir}/applications/

%files
%doc README.md LICENSE
%{_bindir}/*
%{_libdir}/%{name}
%{_datadir}/%{name}
%{_datadir}/applications/triumph4php.desktop

%changelog
* Sun Feb 23 2014 roberto
- 
