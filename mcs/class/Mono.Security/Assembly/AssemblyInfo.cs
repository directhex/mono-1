//
// AssemblyInfo.cs
//
// Authors:
//	Andreas Nahr (ClassDevelopment@A-SoftTech.com)
//	Sebastien Pouliot  (sebastien@ximian.com)
//
// (C) 2003 Ximian, Inc.  http://www.ximian.com
// (C) 2004 Novell (http://www.novell.com)
//

//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

using System;
using System.Reflection;
using System.Resources;
using System.Security;
using System.Security.Permissions;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// General Information about the system assembly

[assembly: AssemblyVersion (Consts.FxVersion)]

[assembly: AssemblyCompany ("MONO development team")]
[assembly: AssemblyCopyright ("(c) 2003-2004 Various Authors")]
[assembly: AssemblyDescription ("Mono.Security.dll")]
[assembly: AssemblyProduct ("MONO CLI")]
[assembly: AssemblyTitle ("Mono.Security.dll")]
[assembly: CLSCompliant (true)]
[assembly: ComVisible (false)]
[assembly: NeutralResourcesLanguage ("en-US")]


// BigInteger use unsafe code
// incomplete - prevent testing on Windows
//[assembly:SecurityPermission (SecurityAction.RequestOptional, UnmanagedCode=true)]


[assembly: AssemblyDelaySign (true)]

#if MOBILE
[assembly: InternalsVisibleTo ("System, PublicKey=00240000048000009400000006020000002400005253413100040000110000009133d396ca929938bc68440b541d8888614e7bd475bef719ab4f4b85b1c21fc3ef2b5f32df0de7c769ca90687650da49eebe7adcb71479f1463e10902cb65a1f44fa2e71b3f30e108fe0f6699d179daed5b1a774db1aba104c59118544b47cda724aea8e6899fa760dfe0ba8656515b48ae94fde29fc8f5bd569126c7a9ae6f3")]
[assembly: AssemblyKeyFile ("../psm.pub")]
#else
[assembly: AssemblyKeyFile ("../mono.pub")]
#endif
