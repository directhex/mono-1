//
// AssemblyInfo.cs
//
// Author:
//   Andreas Nahr (ClassDevelopment@A-SoftTech.com)
//
// (C) 2003 Ximian, Inc.  http://www.ximian.com
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
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

// General Information about the System.Xml assembly

[assembly: AssemblyTitle ("System.Xml.dll")]
[assembly: AssemblyDescription ("System.Xml.dll")]
[assembly: AssemblyDefaultAlias ("System.Xml.dll")]

[assembly: AssemblyCompany (Consts.MonoCompany)]
[assembly: AssemblyProduct (Consts.MonoProduct)]
[assembly: AssemblyCopyright (Consts.MonoCopyright)]
[assembly: AssemblyVersion (Consts.FxVersion)]
[assembly: SatelliteContractVersion (Consts.FxVersion)]
[assembly: AssemblyInformationalVersion (Consts.FxFileVersion)]

[assembly: NeutralResourcesLanguage ("en-US")]

[assembly: ComVisible (false)]

#if !TARGET_JVM
	[assembly: CLSCompliant (true)]
	[assembly: AssemblyDelaySign (true)]
#if NET_2_1
	[assembly: AssemblyKeyFile ("../psm.pub")]
#else
	[assembly: AssemblyKeyFile ("../ecma.pub")]
#endif
#endif

#if NET_2_1
	[assembly: InternalsVisibleTo ("System.Runtime.Serialization, PublicKey=00240000048000009400000006020000002400005253413100040000110000009133d396ca929938bc68440b541d8888614e7bd475bef719ab4f4b85b1c21fc3ef2b5f32df0de7c769ca90687650da49eebe7adcb71479f1463e10902cb65a1f44fa2e71b3f30e108fe0f6699d179daed5b1a774db1aba104c59118544b47cda724aea8e6899fa760dfe0ba8656515b48ae94fde29fc8f5bd569126c7a9ae6f3")]
	[assembly: InternalsVisibleTo ("System.ServiceModel.Web, PublicKey=00240000048000009400000006020000002400005253413100040000110000009133d396ca929938bc68440b541d8888614e7bd475bef719ab4f4b85b1c21fc3ef2b5f32df0de7c769ca90687650da49eebe7adcb71479f1463e10902cb65a1f44fa2e71b3f30e108fe0f6699d179daed5b1a774db1aba104c59118544b47cda724aea8e6899fa760dfe0ba8656515b48ae94fde29fc8f5bd569126c7a9ae6f3")]
	// System.Xml.Serialization (from Microsoft Silverlight 2.0 SDK) requires access to System.Xml internals
	[assembly: InternalsVisibleTo ("System.Xml.Serialization, PublicKey=00240000048000009400000006020000002400005253413100040000110000009133d396ca929938bc68440b541d8888614e7bd475bef719ab4f4b85b1c21fc3ef2b5f32df0de7c769ca90687650da49eebe7adcb71479f1463e10902cb65a1f44fa2e71b3f30e108fe0f6699d179daed5b1a774db1aba104c59118544b47cda724aea8e6899fa760dfe0ba8656515b48ae94fde29fc8f5bd569126c7a9ae6f3")]
#else
	[assembly: AllowPartiallyTrustedCallers]
#endif

#if NET_2_0
	[assembly: AssemblyFileVersion (Consts.FxFileVersion)]
	[assembly: CompilationRelaxations (CompilationRelaxations.NoStringInterning)]
//	[assembly: InternalsVisibleTo ("System.Data.SqlXml, PublicKey=00000000000000000400000000000000")]
	[assembly: Debuggable (DebuggableAttribute.DebuggingModes.IgnoreSymbolStoreSequencePoints)]
	[assembly: RuntimeCompatibility (WrapNonExceptionThrows = true)]
#elif NET_1_1
	[assembly: AssemblyTrademark ("")]
	[assembly: AssemblyConfiguration ("")]
#elif NET_1_0
	[assembly: AssemblyTrademark ("")]
	[assembly: AssemblyConfiguration ("")]
#endif
