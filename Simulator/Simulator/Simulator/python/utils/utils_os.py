import os


'''
Returns the every file that has the extension @extension and is
 in @folder or any of its subfolders
'''
def get_file_with_extension(folder, extension, full_path=True):
    elements = [os.path.join(folder, f) for f in os.listdir(folder)]
    files_of_interest = []

    for element in elements:
        if os.path.isfile(element) == True and element.endswith(extension) == True:
            files_of_interest.append(element)
        elif os.path.isdir(element):
            sub_files_of_interset = get_file_with_extension(element, extension, full_path)
            files_of_interest = files_of_interest + sub_files_of_interset

    return files_of_interest
