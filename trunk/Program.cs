using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml.Linq;
using System.IO;

namespace XorPreCompile
{
	class Program
	{
		static Random random;

		static void Main(string[] args)
		{
			Console.WriteLine("XorPreProcess by KN4CK3R");
			Console.WriteLine("www.oldschoolhack.de");

			if (args.Length < 2)
			{
				Console.Write("XorPreProcess <path to project file> <project file name> [refresh]");
				return;
			}

			//Load Project File
			FileInfo projectFile = new FileInfo(Path.Combine(args[0], args[1]));
			if (projectFile.Exists)
			{
				List<FileInfo> processFiles = new List<FileInfo>();

				if (projectFile.Extension.ToLower() == ".vcxproj") //VS10
				{
					XElement root = XElement.Load(projectFile.OpenRead());
					//foreach (XNode node in root.Elements("ItemGroup"))
					foreach (XElement itemGroup in root.Nodes())
					{
						if (itemGroup.Name.LocalName == "ItemGroup")
						{
							if ((itemGroup.FirstNode as XElement).Name.LocalName == "ClCompile" || (itemGroup.FirstNode as XElement).Name.LocalName == "ClInclude")
							{
								//foreach (var item in itemGroup.Elements("ClCompile"))
								foreach (XElement item in itemGroup.Nodes())
								{
									XAttribute Include = item.Attribute("Include");
									if (Include != null)
									{
										FileInfo file = new FileInfo(Path.Combine(args[0], Include.Value));
										if (file.Exists && ValidFileExtension(file.Extension.ToLower()))
										{
											processFiles.Add(file);
										}
									}
								}
							}
						}
					}
				}
				else if (projectFile.Extension.ToLower() == ".vcproj")
				{
					XElement root = XElement.Load(projectFile.OpenRead());
					//foreach (XNode node in root.Elements("Files"))
					foreach (XElement files in root.Nodes())
					{
						if (files.Name.LocalName == "Files")
						{
							foreach (XElement filter in files.Nodes())
							{
								ListFilesVS2008(processFiles, args[0], filter);
							}
						}
					}
				}
				else
				{
					Console.Write("Error: unknown project file");
					return;
				}

				random = new Random();

				Regex xorRegex;
				if (args.Length == 3 && args[2].ToLower() == "refresh")
				{
					xorRegex = new Regex("_xor_\\(\"(.*?)\"\\)|/\\*(.*?)\\*/XorStr\\(.*?\\)", RegexOptions.Compiled | RegexOptions.Multiline);
				}
				else
				{
					xorRegex = new Regex("_xor_\\(\"(.*?)\"\\)", RegexOptions.Compiled | RegexOptions.Multiline);
				}

				foreach (var file in processFiles)
				{
					StringBuilder sb = new StringBuilder();
					using (StreamReader sr = new StreamReader(file.OpenRead()))
					{
						sb.Append(sr.ReadToEnd());
					}

					if (xorRegex.IsMatch(sb.ToString()))
					{
						try
						{
							Console.WriteLine("xor'ing " + file.Name);
							string result = xorRegex.Replace(sb.ToString(), (MatchEvaluator)delegate(Match match)
							{
								return XorStr(string.IsNullOrEmpty(match.Groups[1].Value) ? match.Groups[2].Value : match.Groups[1].Value);
							});

							using (StreamWriter sw = new StreamWriter(file.OpenWrite()))
							{
								sw.Write(result);
							}
						}
						catch (Exception ex)
						{
							Console.WriteLine("Error: " + ex.Message);
						}
					}
				}
			}
		}

		static void ListFilesVS2008(List<FileInfo> files, string path, XElement filter)
		{
			foreach (XElement item in filter.Nodes())
			{
				XAttribute RelativePath = item.Attribute("RelativePath");
				if (RelativePath != null)
				{
					FileInfo file = new FileInfo(Path.Combine(path, RelativePath.Value.Substring(2)));
					if (file.Exists && ValidFileExtension(file.Extension.ToLower()))
					{
						files.Add(file);
					}
				}
				else if (item.Name.LocalName == "Filter")
				{
					ListFilesVS2008(files, path, item);
				}
			}
		}

		static bool ValidFileExtension(string extension)
		{
			string[] validFileExtensions = { ".cpp", ".c", ".cxx", ".h", ".hpp", ".hxx" };

			foreach (string ext in validFileExtensions)
			{
				if (ext == extension)
				{
					return true;
				}
			}
			return false;
		}

		static string XorStr(string data)
		{
			string realData = string.Empty;

			for (var i = 0; i < data.Length; ++i)
			{
				var c = data[i];
				if (c == '\\')
				{
					++i;
					c = data[i];
					if (c == '\\')
						c = '\\';
					else if (c == 'n')
						c = '\n';
					else if (c == 'r')
						c = '\r';
					else if (c == 't')
						c = '\t';
					else if (c == 'f')
						c = '\f';
					else if (c == 'b')
						c = '\b';
					else if (c == '"')
						c = '\"';
					else if (c == 'x')
					{
						++i;
						if (i >= data.Length)
						{
							throw new Exception("invalid escape sequence: \\x");
						}
						c = char.ToLower(data[i]);
						if (!(('0' <= c && c <= '9') || ('a' <= c && c <= 'f')))
						{
							throw new Exception("invalid escape sequence: \\x" + c);
						}
						string temp = string.Empty + c;
						if (i + 1 < data.Length)
						{
							++i;
							c = char.ToLower(data[i]);
							if (!(('0' <= c && c <= '9') || ('a' <= c && c <= 'f')))
							{
								throw new Exception("invalid escape sequence: \\x" + data[i - 1] + c);
							}
							temp += c;
						}
						c = (char)int.Parse(temp, System.Globalization.NumberStyles.HexNumber);
					}
					else
					{
						throw new Exception("invalid escape sequence: \\" + c);
					}
				}
				realData += c;
			}

			var xor = random.Next(256);

			StringBuilder sb = new StringBuilder();
			sb.Append("/*" + data + "*/XorStr(" + realData.Length + ", 0x" + xor.ToString("X2") + ", ");

			int length = realData.Length / 4;
			for (var i = 0; i < length; ++i)
			{
				sb.Append("0x");
				for (var j = 0; j < 4; ++j)
				{
					var val = (int)realData[i * 4 + j];
					val ^= xor;
					xor += 127;
					xor %= 256;
					sb.Append(val.ToString("X2"));
				}
				sb.Append(", ");
			}
			realData = realData.Substring(length * 4, realData.Length % 4);
			sb.Length -= 2;

			if (realData.Length % 4 != 0)
			{
				sb.Append(", 0x");

				for (var i = 0; i < realData.Length % 4; ++i)
				{
					var val = (int)realData[i];
					val ^= xor;
					xor += 127;
					xor %= 256;
					sb.Append(val.ToString("X2"));
				}

				for (var i = 0; i < 4 - realData.Length % 4; ++i)
				{
					sb.Append("00");
				}
			}

			sb.Append(")");

			return sb.ToString();
		}
	}
}
